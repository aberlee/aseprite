// ASE gui library
// Copyright (C) 2001-2011  David Capello
//
// This source file is ditributed under a BSD-like license, please
// read LICENSE.txt for more information.

#include "config.h"

#include "base/thread.h"

#ifdef WIN32
  #include <windows.h>
  #include <process.h>
#else
  #include <pthread.h>		// Use pthread library in Unix-like systems
#endif

namespace {

#ifdef WIN32

  static DWORD WINAPI win32_thread_proxy(LPVOID data)
  {
    base::thread::details::thread_proxy(data);
    return 0;
  }

#else

  static void* pthread_thread_proxy(void* data)
  {
    base::thread::details::thread_proxy(data);
    return NULL;
  }

#endif

}

base::thread::thread()
  : m_native_handle((native_handle_type)0)
{
}

base::thread::~thread()
{
  if (joinable())
    detach();
}

bool base::thread::joinable() const
{
  return m_native_handle != (native_handle_type)0;
}

void base::thread::join()
{
  if (joinable()) {
#ifdef WIN32
    ::WaitForSingleObject(m_native_handle, INFINITE);
#else
    ::pthread_join((pthread_t)m_native_handle, NULL);
#endif
    detach();
  }
}

void base::thread::detach()
{
  if (joinable()) {
#ifdef WIN32
    ::CloseHandle(m_native_handle);
    m_native_handle = (native_handle_type)0;
#else
    ::pthread_detach((pthread_t)m_native_handle);
#endif
  }
}

base::thread::native_handle_type base::thread::native_handle()
{
  return m_native_handle;
}

void base::thread::launch_thread(func_wrapper* f)
{
  m_native_handle = (native_handle_type)0;

#ifdef WIN32

  DWORD native_id;
  m_native_handle = ::CreateThread(NULL, 0, win32_thread_proxy, (LPVOID)f,
				   CREATE_SUSPENDED, &native_id);
  ResumeThread(m_native_handle);

#else

  pthread_t thread;
  if (::pthread_create(&thread, NULL, pthread_thread_proxy, f) == 0)
    m_native_handle = (void*)thread;

#endif
}

void base::thread::details::thread_proxy(void* data)
{
  func_wrapper* f = reinterpret_cast<func_wrapper*>(data);

  // Call operator() of func_wrapper class (this is a virtual method).
  (*f)();

  // Delete the data (it was created in the thread() ctor).
  delete f;
}

void base::this_thread::yield()
{
#ifdef WIN32
  ::Sleep(0);
#else
  // TODO
#endif
}

void base::this_thread::sleep_for(int milliseconds)
{
#ifdef WIN32
  ::Sleep(milliseconds);
#else
  // TODO
#endif
}
