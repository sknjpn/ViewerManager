#pragma once

class Viewer;

// 表示を行うViewerの保存と更新
class ViewerManager
{
public:
	Array<std::shared_ptr<Viewer>>	m_viewers;
	Array<std::shared_ptr<Viewer>>	m_newViewers;

public:
	ViewerManager() { m_viewers.reserve(1024); }

	void	update();

	template <typename T, typename... Args>
	std::shared_ptr<T>	makeViewer(Args&& ...args) { auto v = dynamic_pointer_cast<T>(m_newViewers.emplace_back(MakeShared<T>(args...))); v->Init(); return v; }

	template<typename T>
	std::shared_ptr<T> getViewer() const
	{
		for (const auto& v : m_viewers)
			if (dynamic_pointer_cast<T>(v) != nullptr) return dynamic_pointer_cast<T>(v);

		for (const auto& v : m_newViewers)
			if (dynamic_pointer_cast<T>(v) != nullptr) return dynamic_pointer_cast<T>(v);

		return nullptr;
	}

	template <typename T>
	bool	hasViewer() const { return getViewer<T>() != nullptr; }

	template <typename T>
	void	deleteViewer()
	{
		// nullptrにすることで、配列の配置を変えない
		for (auto& v : m_viewers)
			if (dynamic_pointer_cast<T>(v) != nullptr) v = nullptr;

		m_newViewers.erase(remove_if(m_newViewers.begin(), m_newViewers.end(), [](const auto& v) { return dynamic_pointer_cast<T>(v) != nullptr; }), m_newViewers.end());
	}

	void	clearViewrs()
	{
		// nullptrにすることで、配列の配置を変えない
		for (auto& v : m_viewers) v = nullptr;
	}
};

extern std::unique_ptr<ViewerManager>	g_viewerManagerPtr;
