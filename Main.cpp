﻿#include "ViewerManager.h"

class TitleViewer
	: public Viewer
{
	Font	m_font;
	Vec2	m_m_catPos;
	Texture m_cat;
	Color	m_color;

public:
	void init() override
	{
		// 背景を水色にする
		setBackgroundColor(ColorF(0.8, 0.9, 1.0));

		// 大きさ 60 のフォントを用意
		m_font = Font(60);

		// 猫のテクスチャを用意
		m_cat = Texture(Emoji(U"🐈"));

		// 猫の座標
		m_m_catPos.set(400, 450);
		m_color = Palette::White;

		setDrawRect(600, 600);
	}

	void update() override
	{
		// テキストを画面の中心に描く
		m_font(U"Hello, Siv3D!🐣").drawAt(Scene::Center(), Palette::Black);

		// 大きさをアニメーションさせて猫を表示する
		m_cat.resized(100 + Periodic::Sine0_1(1s) * 20).drawAt(m_m_catPos, m_color);

		// マウスカーソルに追従する半透明の赤い円を描く
		Circle(Cursor::Pos(), 40).draw(ColorF(1, 0, 0, 0.5));

		// [A] キーが押されたら
		if (KeyA.down())
		{
			// Hello とデバッグ表示する
			Print << U"Hello!";
		}

	}

	void	SetColor(const Color& color) { m_color = color; }
	void	SetCatPos(const Vec2& pos) { m_m_catPos = pos; }
};

class ButtonViewer
	: public Viewer
{
public:
	void init() override
	{
		setDrawRect(600, 0, 200, 300);
		setBackgroundColor(Palette::Gray);
	}

	void update() override
	{
		if (SimpleGUI::Button(U"White", Vec2(10, 10), 180))
		{
			ViewerManager::GetViewer<TitleViewer>()->SetColor(Palette::White);
		}

		moveDrawPos(0, 40);
		if (SimpleGUI::Button(U"Red", Vec2(10, 10), 180))
		{
			ViewerManager::GetViewer<TitleViewer>()->SetColor(Palette::Red);
		}

		moveDrawPos(0, 40);
		if (SimpleGUI::Button(U"Green", Vec2(10, 10), 180))
		{
			ViewerManager::GetViewer<TitleViewer>()->SetColor(Palette::Green);
		}

		moveDrawPos(0, 40);
		if (SimpleGUI::Button(U"Blue", Vec2(10, 10), 180))
		{
			ViewerManager::GetViewer<TitleViewer>()->SetColor(Palette::Blue);
		}
	}
};

class TeleportViewer
	: public Viewer
{
public:
	void init() override
	{
		setDrawRect(600, 300, 200, 300);
		setBackgroundColor(Palette::Gray);
	}

	void update() override
	{
		// ボタンが押されたら
		if (SimpleGUI::Button(U"Move the cat", Vec2(10, 10), 180))
		{
			// 猫の座標を画面内のランダムな位置に移動する
			ViewerManager::GetViewer<TitleViewer>()->SetCatPos(RandomVec2(Scene::Rect()));
		}
	}
};

void Main()
{
	ViewerManager::MakeViewer<TitleViewer>();
	ViewerManager::MakeViewer<ButtonViewer>();
	ViewerManager::MakeViewer<TeleportViewer>();

	while (System::Update())
	{
		ViewerManager::Update();
	}
}
