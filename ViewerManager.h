#pragma once

class Viewer
{
	friend class ViewerManager;

	bool	m_isMouseOver = false;
	Vec2	m_drawPos = Vec2::Zero();
	RectF	m_drawRect = RectF(Scene::Size());
	Color	m_backgroundColor = Color(11, 22, 33, 128);
	std::unique_ptr<Transformer2D>	m_transformer;

public:
	virtual ~Viewer() = default;

	// Set
	void	SetBackgroundColor(const Color& color) { m_backgroundColor = color; }
	void	SetDrawPos(const Vec2& pos) { m_drawPos = pos; m_transformer.reset(); m_transformer = MakeUnique<Transformer2D>(Mat3x2::Translate(m_drawPos), true); }
	void	SetDrawPos(double x, double y) { SetDrawPos(Vec2(x, y)); }
	void	SetDrawRect(const Vec2& pos, const Vec2& size) { m_drawRect = RectF(pos, size); }
	void	SetDrawRect(const Vec2& size) { m_drawRect = RectF(size); }
	void	SetDrawRect(double x, double y, double w, double h) { m_drawRect = RectF(x, y, w, h); }
	void	SetDrawRect(double w, double h) { m_drawRect = RectF(w, h); }
	void	MoveDrawPos(double dx, double dy) { SetDrawPos(m_drawPos.movedBy(dx, dy)); }

	// Get
	const RectF& GetDrawRect() const { return m_drawRect; }
	const Vec2& GetDrawPos() const { return m_drawPos; }
	bool	IsMouseOver() const { return m_isMouseOver; }

	virtual void	Init() {}
	virtual void	update() {}
};

// 表示を行うViewerの保存と更新
class ViewerManager
{
public:
	Array<std::shared_ptr<Viewer>>	m_viewers;
	Array<std::shared_ptr<Viewer>>	m_newViewers;

	static ViewerManager& GetInstance()
	{
		static ViewerManager instance;
		return instance;
	}

public:
	ViewerManager() = default;
	~ViewerManager() = default;
	ViewerManager(const ViewerManager&) = delete;
	ViewerManager& operator=(const ViewerManager&) = delete;
	ViewerManager(ViewerManager&&) = delete;
	ViewerManager& operator=(ViewerManager&&) = delete;

	static void	Update()
	{
		auto& instance = ViewerManager::GetInstance();

		// nullptr になったViewerのリセット
		instance.m_viewers.remove(nullptr);

		// Viewerのリセット
		for (auto it = instance.m_viewers.begin(); it < instance.m_viewers.end(); ++it)
		{
			(*it)->m_isMouseOver = false;
			(*it)->m_drawPos = Vec2::Zero();
		}

		// mouseOver 適用
		for (auto it = instance.m_viewers.rbegin(); it < instance.m_viewers.rend(); ++it)
		{
			if ((*it)->m_drawRect.mouseOver())
			{
				(*it)->m_isMouseOver = true;

				break;
			}
		}

		// Viewer 更新
		for (auto it = instance.m_viewers.begin(); it < instance.m_viewers.end(); ++it)
		{
			// Viewerが消されてnullptrになっている可能性がある
			if (*it == nullptr) continue;

			auto vp = ScopedViewport2D(Rect((*it)->m_drawRect));
			auto t = Transformer2D(Mat3x2::Identity(), Mat3x2::Translate((*it)->m_drawRect.pos));

			RectF((*it)->m_drawRect.size).draw((*it)->m_backgroundColor);

			(*it)->update();

			// Viewerが消されてnullptrになっている可能性がある
			if (*it != nullptr) (*it)->m_transformer.reset();

			RectF((*it)->m_drawRect.size).drawFrame(1.0, 0.0, ColorF(Palette::Red, 0.5));
		}

		// Viewer追加
		for (const auto& v : instance.m_newViewers)
			instance.m_viewers.emplace_back(v);
		instance.m_newViewers.clear();
	}

	template <typename T, typename... Args>
	static std::shared_ptr<T>	MakeViewer(Args&& ...args) { auto v = dynamic_pointer_cast<T>(GetInstance().m_newViewers.emplace_back(MakeShared<T>(args...))); v->Init(); return v; }

	template<typename T>
	static std::shared_ptr<T> GetViewer()
	{
		auto& instance = GetInstance();

		for (const auto& v : instance.m_viewers)
			if (dynamic_pointer_cast<T>(v) != nullptr) return dynamic_pointer_cast<T>(v);

		for (const auto& v : instance.m_newViewers)
			if (dynamic_pointer_cast<T>(v) != nullptr) return dynamic_pointer_cast<T>(v);

		return nullptr;
	}

	template <typename T>
	static bool	HasViewer() { return GetInstance().GetViewer<T>() != nullptr; }

	template <typename T>
	static void	DeleteViewer()
	{
		auto& instance = GetInstance();

		// nullptrにすることで、配列の配置を変えない
		for (auto& v : instance.m_viewers)
			if (dynamic_pointer_cast<T>(v) != nullptr) v = nullptr;

		instance.m_newViewers.erase(remove_if(instance.m_newViewers.begin(), instance.m_newViewers.end(), [](const auto& v) { return dynamic_pointer_cast<T>(v) != nullptr; }), instance.m_newViewers.end());
	}

	static void	ClearViewrs()
	{
		// nullptrにすることで、配列の配置を変えない
		for (auto& v : GetInstance().m_viewers) v = nullptr;
	}
};
