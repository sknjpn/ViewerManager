#pragma once

class Viewer;

// 表示を行うViewerの保存と更新
class ViewerManager
{
public:
	Array<std::shared_ptr<Viewer>>	m_viewers;
	Array<std::shared_ptr<Viewer>>	m_new_viewers;

public:
	ViewerManager() { m_viewers.reserve(1024); }

	void	Update();

	template <typename T, typename... Args>
	std::shared_ptr<T>	MakeViewer(Args&& ...args) { auto v = dynamic_pointer_cast<T>(m_new_viewers.emplace_back(MakeShared<T>(args...))); v->Init(); return v; }

	template<typename T>
	std::shared_ptr<T> GetViewer() const
	{
		for (const auto& v : m_viewers)
			if (dynamic_pointer_cast<T>(v) != nullptr) return dynamic_pointer_cast<T>(v);

		for (const auto& v : m_new_viewers)
			if (dynamic_pointer_cast<T>(v) != nullptr) return dynamic_pointer_cast<T>(v);

		return nullptr;
	}

	template <typename T>
	bool	HasViewer() const { return GetViewer<T>() != nullptr; }

	template <typename T>
	void	DeleteViewer()
	{
		// nullptrにすることで、配列の配置を変えない
		for (auto& v : m_viewers)
			if (dynamic_pointer_cast<T>(v) != nullptr) v = nullptr;

		m_new_viewers.erase(remove_if(m_new_viewers.begin(), m_new_viewers.end(), [](const auto& v) { return dynamic_pointer_cast<T>(v) != nullptr; }), m_new_viewers.end());
	}

	void	ClearViewers()
	{
		// nullptrにすることで、配列の配置を変えない
		for (auto& v : m_viewers) v = nullptr;
	}
};

extern std::unique_ptr<ViewerManager>	g_viewerManagerPtr;
