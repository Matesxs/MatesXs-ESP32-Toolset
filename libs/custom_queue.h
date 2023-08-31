#pragma once

#include <Arduino.h>

template<class T, size_t size>
class Queue
{
public:
	Queue() 
  {
    m_semaphore = xSemaphoreCreateMutex();
  }

	bool Enque(T val)
	{
    xSemaphoreTake(m_semaphore, portMAX_DELAY);
		if (IsFull())
    {
      xSemaphoreGive(m_semaphore);
      return false;
    }

		m_data[m_rear] = val;
		m_rear = (m_rear + 1) % size;
		m_used++;

    xSemaphoreGive(m_semaphore);
		return true;
	}

	bool Deque(T* ret)
	{
    xSemaphoreTake(m_semaphore, portMAX_DELAY);
		if (IsEmpty())
    {
      xSemaphoreGive(m_semaphore);
      return false;
    }

		*ret = m_data[m_front];
		m_front = (m_front + 1) % size;
		m_used--;

    xSemaphoreGive(m_semaphore);
		return true;
	}

	bool Deque()
	{
    xSemaphoreTake(m_semaphore, portMAX_DELAY);
		if (IsEmpty())
    {
      xSemaphoreGive(m_semaphore);
      return false;
    }

		m_front = (m_front + 1) % size;
		m_used--;

    xSemaphoreGive(m_semaphore);
		return true;
	}

	bool Peek(T* ret)
	{
    xSemaphoreTake(m_semaphore, portMAX_DELAY);
		if (IsEmpty())
    {
      xSemaphoreGive(m_semaphore);
      return false;
    }

		*ret = m_data[m_front];

    xSemaphoreGive(m_semaphore);
		return true;
	}

  void Clear()
  {
    xSemaphoreTake(m_semaphore, portMAX_DELAY);
    m_used = 0;
    m_front = 0;
    m_rear = 0;
    xSemaphoreGive(m_semaphore);
  }

	size_t Size() const { return m_used; }
	bool IsFull() const { return m_used >= size; }
	bool IsEmpty() const { return m_used == 0; }
private:
	T m_data[size];
	size_t m_used = 0;
	size_t m_front = 0;
	size_t m_rear = 0;
  SemaphoreHandle_t m_semaphore;
};
