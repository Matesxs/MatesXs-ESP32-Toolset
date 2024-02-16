#pragma once

template<class T>
class VariableStore
{
public:
  VariableStore() {};
  VariableStore(T value) { m_Value = value; m_Stored = true; };
  bool IsUsed() { return m_Stored; }
  void Set(T value) { m_Stored=true; m_Value=value; }
  void Reset() { m_Stored=false; }
  T Get() const { return m_Value; }
  operator T() const { return m_Value; }
  VariableStore& operator= (const T value) { Set(value); return *this; }

private:
  T m_Value;
  bool m_Stored = false;
};
