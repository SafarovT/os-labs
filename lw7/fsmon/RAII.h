#pragma once
#include <unistd.h>
#include <sys/inotify.h>

class InotifyFileDescriptorRAII
{
public:
	InotifyFileDescriptorRAII()
		: m_inotifyFd(inotify_init())
	{}

	~InotifyFileDescriptorRAII()
	{
		close(m_inotifyFd);
	}

	int GetFileDescriptor() const
	{
		return m_inotifyFd;
	}

private:
	int m_inotifyFd;

	InotifyFileDescriptorRAII(const InotifyFileDescriptorRAII& p) {}
};

class WatchDescriptorRAII
{
public:
	WatchDescriptorRAII(int inotifyFd, const char* path)
		: m_inotifyFd(inotifyFd)
	{
		m_watchDescriptor = inotify_add_watch
		(
			m_inotifyFd,
			path,
			IN_CREATE
			| IN_DELETE
			| IN_MODIFY
			| IN_ATTRIB
			| IN_MOVED_FROM
			| IN_MOVED_TO
		);
	}

	~WatchDescriptorRAII()
	{
		inotify_rm_watch(m_inotifyFd, m_watchDescriptor);
	}

	int GetWatchDescriptor() const
	{
		return m_watchDescriptor;
	}

private:
	int m_inotifyFd;
	int m_watchDescriptor;

	WatchDescriptorRAII(const WatchDescriptorRAII& p) {}
};