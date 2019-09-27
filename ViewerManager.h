#pragma once

class Viewer
{
	friend class ViewerManager;

	bool	m_isMouseOver = false;
	Vec2	m_drawPos = Vec2::Zero();
	RectF	m_drawRect = RectF(Scene::Size());
	Color	m_backgroundColor = Color(11, 22, 33, 255);
	std::unique_ptr<Transformer2D>	m_transformer;

public:
	virtual ~Viewer() = default;

	// Set
	void	setBackgroundColor(const Color& color) { m_backgroundColor = color; }
	void	setDrawPos(const Vec2& pos) { m_drawPos = pos; m_transformer.reset(); m_transformer = MakeUnique<Transformer2D>(Mat3x2::Translate(m_drawPos), true); }
	void	setDrawPos(double x, double y) { setDrawPos(Vec2(x, y)); }
	void	setDrawRect(const Vec2& pos, const Vec2& size) { m_drawRect = RectF(pos, size); }
	void	setDrawRect(const Vec2& size) { m_drawRect = RectF(size); }
	void	setDrawRect(double x, double y, double w, double h) { m_drawRect = RectF(x, y, w, h); }
	void	setDrawRect(double w, double h) { m_drawRect = RectF(w, h); }
	void	moveDrawPos(double dx, double dy) { setDrawPos(m_drawPos.movedBy(dx, dy)); }

	// Get
	const RectF& getDrawRect() const { return m_drawRect; }
	const Vec2& getDrawPos() const { return m_drawPos; }
	bool	isMouseOver() const { return m_isMouseOver; }

	virtual void	init() {}
	virtual void	update() {}
};

// 表示を行うViewerの保存と更新
class ViewerManager
{
	Array<std::shared_ptr<Viewer>>	m_viewers;
	Array<std::shared_ptr<Viewer>>	m_newViewers;

private:
	static ViewerManager& GetInstance()
	{
		static ViewerManager instance;

		return instance;
	}

	template <typename T>
	void	deleteViewer()
	{
		// nullptrにすることで、配列の配置を変えない
		for (auto& v : m_viewers)
			if (dynamic_pointer_cast<T>(v) != nullptr) v = nullptr;

		m_newViewers.erase(remove_if(m_newViewers.begin(), m_newViewers.end(), [](const auto& v) { return dynamic_pointer_cast<T>(v) != nullptr; }), m_newViewers.end());
	}

	void update()
	{
		// nullptr になったViewerのリセット
		m_viewers.remove(nullptr);

		// Viewerのリセット
		for (auto it = m_viewers.begin(); it < m_viewers.end(); ++it)
		{
			(*it)->m_isMouseOver = false;
			(*it)->m_drawPos = Vec2::Zero();
		}

		// mouseOver 適用
		for (auto it = m_viewers.rbegin(); it < m_viewers.rend(); ++it)
		{
			if ((*it)->m_drawRect.mouseOver())
			{
				(*it)->m_isMouseOver = true;

				break;
			}
		}

		// Viewer 更新
		for (auto it = m_viewers.begin(); it < m_viewers.end(); ++it)
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
		for (const auto& v : m_newViewers)
			m_viewers.emplace_back(v);
		m_newViewers.clear();
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
		ViewerManager::GetInstance().update();
	}

	template <typename T, typename... Args>
	static std::shared_ptr<T>	MakeViewer(Args&& ...args)
	{
		auto v = dynamic_pointer_cast<T>(GetInstance().m_newViewers.emplace_back(MakeShared<T>(args...)));

		v->init();

		return v;
	}

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
	static bool	HasViewer()
	{
		return GetInstance().GetViewer<T>() != nullptr;
	}

	template <typename T>
	static void	DeleteViewer()
	{
		GetInstance().deleteViewer();
	}

	static void	ClearViewrs()
	{
		// nullptrにすることで、配列の配置を変えない
		for (auto& v : GetInstance().m_viewers) v = nullptr;
	}
};
