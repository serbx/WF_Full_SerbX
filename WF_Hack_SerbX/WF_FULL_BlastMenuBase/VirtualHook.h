#pragma once
#include "VirtualUtilite.h"

class VirtualhkCRT
{
public:
	bool bInitialize(_pdword* ppdwClassBase);
	bool ReLoad(_pdword* ppdwClassBase);
	bool bInitialize(_pdword** pppdwClassBase);
	void UnHook(void);
	void ReHook(void);
	int iGetFuncCount(void);
	_dword dwGetMethodAddress(int Index);
	_pdword pdwGetOldVMT(void);
	_pdword pdwGetNewVMT(void);
	_dword dwHookMethod(_dword dwNewFunc, unsigned int iIndex);
	template <class cData> cData GetOrigFuntion(UINT index)
	{
		return (cData)m_pdwOldVMT[index];
	}
private:
	_pdword* m_ppdwClassBase;
	_dword	m_pdwNewVMT[175];
	_pdword	m_pdwOldVMT;
	_dword	m_dwVMTSize;
};
class VirtualhkUpdate
{
public:
	VirtualhkUpdate(void);
	VirtualhkUpdate(_pdword* ppdwClassBase);
	~VirtualhkUpdate(void);
	bool bInitialize(_pdword* ppdwClassBase);
	bool bInitialize(_pdword** pppdwClassBase);
	void UnHook(void);
	void ReHook(void);
	int iGetFuncCount(void);
	_dword dwGetMethodAddress(int Index);
	_pdword pdwGetOldVMT(void);
	_dword dwHookMethod(_dword dwNewFunc, unsigned int iIndex);
	bool bIsValid();
	template <class cData> cData GetOrigFuntion(UINT index)
	{
		return (cData)m_pdwOldVMT[index];
	}

private:
	_dword dwGetVMTCount(_pdword pdwVMT)
	{
		_dword dwIndex = 0;

		for (dwIndex = 0; pdwVMT[dwIndex]; dwIndex++)
		{
			if (IsBadCodePtr((FARPROC)pdwVMT[dwIndex]))
			{
				break;
			}
		}
		return dwIndex;
	}

	_pdword* m_ppdwClassBase = NULL;
	_pdword	m_pdwNewVMT, m_pdwOldVMT;
	_dword	m_dwVMTSize;
};

class VMTHookManager
{
public:
    VMTHookManager(void)
    {
        memset(this, 0, sizeof(VMTHookManager));
    }
    VMTHookManager(_pdword* ppdwClassBase)
    {
        bInitialize(ppdwClassBase);
    }
    ~VMTHookManager(void)
    {
        UnHook();
    }
    bool bInitialize(_pdword* ppdwClassBase)
    {
        m_ppdwClassBase = ppdwClassBase;
        m_pdwOldVMT = *ppdwClassBase;
        m_dwVMTSize = dwGetVMTCount(*ppdwClassBase);
        m_pdwNewVMT = new _dword[m_dwVMTSize];
        memcpy(m_pdwNewVMT, m_pdwOldVMT, sizeof(_dword) * m_dwVMTSize);
        *ppdwClassBase = m_pdwNewVMT;
        return true;
    }
    bool bInitialize(_pdword** pppdwClassBase)
    {
        return bInitialize(*pppdwClassBase);
    }
    void UnHook(void)
    {
        if (m_ppdwClassBase)
        {
            *m_ppdwClassBase = m_pdwOldVMT;
        }
    }
    void ReHook(void)
    {
        if (m_ppdwClassBase)
        {
            *m_ppdwClassBase = m_pdwNewVMT;
        }
    }
    int iGetFuncCount(void)
    {
        return (int)m_dwVMTSize;
    }
    _dword dwGetMethodAddress(int Index)
    {
        if (Index >= 0 && Index <= (int)m_dwVMTSize && m_pdwOldVMT != NULL)
        {
            return m_pdwOldVMT[Index];
        }
        return NULL;
    }
    _pdword pdwGetOldVMT(void)
    {
        return m_pdwOldVMT;
    }
    _dword dwHookMethod(_dword dwNewFunc, unsigned int iIndex)
    {
        if (m_pdwNewVMT && m_pdwOldVMT && iIndex <= m_dwVMTSize && iIndex >= 0)
        {
            m_pdwNewVMT[iIndex] = dwNewFunc;
            return m_pdwOldVMT[iIndex];
        }

        return NULL;
    }
private:
    _dword dwGetVMTCount(_pdword pdwVMT)
    {
        _dword dwIndex = 0;

        for (dwIndex = 0; pdwVMT[dwIndex]; dwIndex++)
        {
            if (IsBadCodePtr((FARPROC)pdwVMT[dwIndex]))
            {
                break;
            }
        }
        return dwIndex;
    }
    _pdword* m_ppdwClassBase;
    _pdword	m_pdwNewVMT, m_pdwOldVMT;
    _dword	m_dwVMTSize;
};