#include "VirtualHook.h"

bool VirtualhkCRT::bInitialize(_pdword* ppdwClassBase)
{
	m_ppdwClassBase = ppdwClassBase;
	m_pdwOldVMT = *ppdwClassBase;
	m_dwVMTSize = 175;
	memcpy(m_pdwNewVMT, m_pdwOldVMT, sizeof(m_pdwNewVMT));
	*ppdwClassBase = m_pdwNewVMT;
	return true;
}
bool VirtualhkCRT::ReLoad(_pdword* ppdwClassBase)
{
	m_ppdwClassBase = ppdwClassBase;
	m_pdwOldVMT = *ppdwClassBase;
	*ppdwClassBase = m_pdwNewVMT;
	return true;
}
bool VirtualhkCRT::bInitialize(_pdword** pppdwClassBase)
{
	return bInitialize(*pppdwClassBase);
}
void VirtualhkCRT::UnHook(void)
{
	if (m_ppdwClassBase)*m_ppdwClassBase = m_pdwOldVMT;
}
void VirtualhkCRT::ReHook(void)
{
	if (m_ppdwClassBase)*m_ppdwClassBase = m_pdwNewVMT;
}
int VirtualhkCRT::iGetFuncCount(void)
{
	return (int)m_dwVMTSize;
}
_dword VirtualhkCRT::dwGetMethodAddress(int Index)
{
	if (Index >= 0 && Index <= (int)m_dwVMTSize && m_pdwOldVMT != NULL)
		return m_pdwOldVMT[Index];
	return NULL;
}
_pdword VirtualhkCRT::pdwGetOldVMT(void) { return m_pdwOldVMT; }
_pdword VirtualhkCRT::pdwGetNewVMT(void) { return m_pdwNewVMT; }
_dword VirtualhkCRT::dwHookMethod(_dword dwNewFunc, unsigned int iIndex)
{
	if (m_pdwNewVMT && m_pdwOldVMT && iIndex <= m_dwVMTSize && iIndex >= 0)
	{
		m_pdwNewVMT[iIndex] = dwNewFunc;
		return m_pdwOldVMT[iIndex];
	}
	return NULL;
}

VirtualhkUpdate::VirtualhkUpdate(void)
{
	memset(this, 0, sizeof(VirtualhkUpdate));
}
VirtualhkUpdate::VirtualhkUpdate(_pdword* ppdwClassBase)
{
	bInitialize(ppdwClassBase);
}
VirtualhkUpdate::~VirtualhkUpdate(void)
{
	UnHook();
}
bool VirtualhkUpdate::bInitialize(_pdword* ppdwClassBase)
{
	m_ppdwClassBase = ppdwClassBase;
	m_pdwOldVMT = *ppdwClassBase;
	m_dwVMTSize = dwGetVMTCount(*ppdwClassBase);
	m_pdwNewVMT = new _dword[m_dwVMTSize];
	memcpy(m_pdwNewVMT, m_pdwOldVMT, sizeof(_dword) * m_dwVMTSize);
	*ppdwClassBase = m_pdwNewVMT;
	return true;
}
bool VirtualhkUpdate::bInitialize(_pdword** pppdwClassBase)
{
	return bInitialize(*pppdwClassBase);
}
void VirtualhkUpdate::UnHook(void)
{
	if (m_ppdwClassBase)
	{
		*m_ppdwClassBase = m_pdwOldVMT;
	}
}
void VirtualhkUpdate::ReHook(void)
{
	if (m_ppdwClassBase)
	{
		*m_ppdwClassBase = m_pdwNewVMT;
	}
}
int VirtualhkUpdate::iGetFuncCount(void)
{
	return (int)m_dwVMTSize;
}
_dword VirtualhkUpdate::dwGetMethodAddress(int Index)
{
	if (Index >= 0 && Index <= (int)m_dwVMTSize && m_pdwOldVMT != NULL)
	{
		return m_pdwOldVMT[Index];
	}
	return NULL;
}
_pdword VirtualhkUpdate::pdwGetOldVMT(void)
{
	return m_pdwOldVMT;
}
_dword VirtualhkUpdate::dwHookMethod(_dword dwNewFunc, unsigned int iIndex)
{
	if (m_pdwNewVMT && m_pdwOldVMT && iIndex <= m_dwVMTSize && iIndex >= 0)
	{
		m_pdwNewVMT[iIndex] = dwNewFunc;
		return m_pdwOldVMT[iIndex];
	}

	return NULL;
}
bool VirtualhkUpdate::bIsValid()
{
	return m_ppdwClassBase && *m_ppdwClassBase;
}