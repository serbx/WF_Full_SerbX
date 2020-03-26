#include "stdafx.h"


void MenuGroup::Render(Renderer* r, float x, float y, float w)
{
	r->SetDrawColor(m_color);
	r->AddFilledRect(DirectX::XMFLOAT4(x, y, w, m_height));
	// Reset height calculation
	m_height = 0.f;

	r->SetDrawColor(m_title_background_color);
	r->AddFilledRect(DirectX::XMFLOAT4(x, y, w, m_title_font.getSize() + 8));

	if (m_selected_item == 0) // Group title is selected
	{
		r->SetDrawColor(m_selected_items_color); // Render group with selected item color
		r->AddFilledRect(DirectX::XMFLOAT4(x, y + 2, w, m_title_font.getSize() + 4));
	}

	r->SetFontColor(m_title_color);
	m_height += 4;
	r->AddText(m_title_font, x + 5, y + 2, m_name);
	m_height += m_title_font.getSize() + 4;

	if (m_minimized)
		return;

	for (size_t i = 0, len = m_items.size(); i < len; ++i)
	{
		Item& item = m_items[i];

		if (i == (m_selected_item - 1))
		{
			r->SetDrawColor(m_selected_items_color);
			r->AddFilledRect(DirectX::XMFLOAT4(x, y + m_height, w, 20));
		}
		else
		{
			r->SetFontColor(m_items_color);
		}


		r->AddText(m_item_font, x + 8, y + m_height + 3, item.Name);
		switch (item.Type)
		{
		case ITEM_BOOL:
			r->AddText(m_item_font, x + w - 8, y + m_height + 3, *item.Value.Check ? "True" : "False", FONT_ALIGN_RIGHT);
			break;
		case ITEM_NUMBER:
			r->AddText(m_item_font, x + w - 8, y + m_height + 3, std::to_string(*item.Value.Numeric.Number), FONT_ALIGN_RIGHT);
			break;
		case ITEM_TEXT:
			r->AddText(m_item_font, x + w - 8, y + m_height + 3, *item.Value.Text, FONT_ALIGN_RIGHT);
			break;
		case ITEM_TEXT_ARRAY:
			auto& value = item.Value.Options;
			if (value == nullptr)
				break;
			if (value->Selected < 0 || value->Selected >= value->TextArray.size() || value->TextArray.size() == 0)
				break;

			r->AddText(m_item_font, x + w - 8, y + m_height + 3, value->TextArray[value->Selected], FONT_ALIGN_RIGHT);
			break;
		}

		m_height += m_item_font.getSize() + 6;
	}
}

void MenuGroup::Navigate()
{
	const size_t len = m_items.size();
	if (len == 0)
		return;

	if (GetAsyncKeyState(VK_DOWN) & 1)
		m_selected_item = (m_selected_item + 1) % (len + 1);
	else if (GetAsyncKeyState(VK_UP) & 1)
		m_selected_item = (m_selected_item - 1) % (len + 1);

	if (m_minimized)
		m_selected_item = 0;

	if (m_selected_item == 0)
	{
		if (GetAsyncKeyState(VK_LEFT) & 1)
			m_minimized = true;
		else if (GetAsyncKeyState(VK_RIGHT) & 1)
			m_minimized = false;

		return;
	}
	
	Item& selected_item = m_items[(m_selected_item - 1) % m_items.size()];
	if (GetAsyncKeyState(VK_LEFT) & 1)
	{
		switch (selected_item.Type)
		{
		case ITEM_BOOL:
			*selected_item.Value.Check = false;
			break;
		case ITEM_NUMBER:
			*selected_item.Value.Numeric.Number = max(selected_item.Value.Numeric.Minimum, *selected_item.Value.Numeric.Number - 1);
			break;
		case ITEM_TEXT_ARRAY:
			OptionValue* const value = selected_item.Value.Options;
			if (value == nullptr)
				break;
			value->Selected = value->Selected != 0 ? value->Selected - 1 : 0;
			break;
		}
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 1)
	{
		switch (selected_item.Type)
		{
		case ITEM_BOOL:
			*selected_item.Value.Check = true;
			break;
		case ITEM_NUMBER:
			*selected_item.Value.Numeric.Number = min(selected_item.Value.Numeric.Maximum, *selected_item.Value.Numeric.Number + 1);
			break;
		case ITEM_TEXT_ARRAY:
			OptionValue* const value = selected_item.Value.Options;
			if (value == nullptr)
				break;
			value->Selected = min(value->TextArray.size() - 1, value->Selected + 1);
			break;
		}
	}
}

void Menu::Render(Renderer* r, float x, float y, float w)
{
	r->SetDrawColor(m_color);
	r->AddFilledRect(DirectX::XMFLOAT4(x, y, w, m_height));
	// Reset height calculation
	m_height = 0.f;

	//r->SetFontColor(m_title_color);
	//r->SetDrawColor(m_title_background_color);
	//r->AddFilledRect(DirectX::XMFLOAT4(x, y, w, m_title_font.getSize() + 6));
	//m_height += 4;
	//r->AddText(m_title_font, x + (w / 2), y + 2, m_title, FONT_ALIGN_CENTER);
	//m_height += m_title_font.getSize() + 10;

	if (m_minimized)
		return;

	for (size_t i = 0, len = m_groups.size(); i < len; ++i)
	{
		MenuGroup& group = m_groups[i];
		group.Render(r, x, y + m_height, w);

		m_height += group.getCalculatedHeight() + 6; // Padding of 5
	}
}

void Menu::Navigate()
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
		toggleMinimized();

	if (isMinimized())
		return;

	const size_t len = m_groups.size();
	if (len == 0)
		return;

	MenuGroup& selected_group = m_groups[m_selected_group];
	const size_t selected_group_item_len = selected_group.m_items.size();
	if (selected_group.getSelectedItemIndex() == 0
		&& GetAsyncKeyState(VK_UP) & 1)
	{
		selected_group.setSelectedItemIndex(-1);
		m_selected_group = m_selected_group != 0 ? m_selected_group - 1 : m_groups.size() - 1;
		auto& new_selected_group = m_groups[m_selected_group];
		new_selected_group.setSelectedItemIndex(new_selected_group.m_items.size());
	}
	else if ((selected_group.getSelectedItemIndex() == selected_group_item_len 
		|| (selected_group.getSelectedItemIndex() == 0 
		&& selected_group.isMinimized()))
		&& GetAsyncKeyState(VK_DOWN) & 1)
	{
		selected_group.setSelectedItemIndex(-1);
		m_selected_group = (m_selected_group + 1) % m_groups.size();
		m_groups[m_selected_group].setSelectedItemIndex(0);
	}
	else
	{
		selected_group.Navigate();
	}
}