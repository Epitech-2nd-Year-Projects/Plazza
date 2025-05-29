#include "Communication/MessageQueue.hpp"
#include "Exceptions/MessageException.hpp"
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace Plazza::Communication {
MessageQueue::MessageQueue(const std::string &queueName, bool isCreator)
    : m_name("/" + queueName), m_descriptor(-1), m_isCreator(isCreator),
      m_isOpen(false) {
  struct mq_attr attr = {};
  attr.mq_flags = 0;
  attr.mq_maxmsg = MAX_MESSAGES;
  attr.mq_msgsize = MAX_MESSAGE_SIZE;
  attr.mq_curmsgs = 0;

  if (m_isCreator) {
    mq_unlink(m_name.c_str());
    m_descriptor =
        mq_open(m_name.c_str(), O_CREAT | O_RDWR | O_NONBLOCK, 0644, &attr);
  } else {
    m_descriptor = mq_open(m_name.c_str(), O_RDWR);
  }

  if (m_descriptor == -1) {
    throw Exceptions::MessageException("Failed to open message queue: " +
                                       m_name + " - " + std::strerror(errno));
  }
  m_isOpen = true;
}

MessageQueue::~MessageQueue() { close(); }

MessageQueue::MessageQueue(MessageQueue &&other) noexcept
    : m_name(std::move(other.m_name)), m_descriptor(other.m_descriptor),
      m_isCreator(other.m_isCreator), m_isOpen(other.m_isOpen) {
  other.m_descriptor = -1;
  other.m_isOpen = false;
}

MessageQueue &MessageQueue::operator=(MessageQueue &&other) noexcept {
  if (this != &other) {
    close();
    m_name = std::move(other.m_name);
    m_descriptor = other.m_descriptor;
    m_isCreator = other.m_isCreator;
    m_isOpen = other.m_isOpen;
    other.m_descriptor = -1;
    other.m_isOpen = false;
  }
  return *this;
}

void MessageQueue::send(const std::string &message, unsigned int priority) {
  if (!m_isOpen || m_descriptor == -1) {
    throw Exceptions::MessageException("Message queue is not open");
  }

  if (message.size() >= MAX_MESSAGE_SIZE) {
    throw Exceptions::MessageException("Message too large");
  }

  if (mq_send(m_descriptor, message.c_str(), message.size(), priority) == -1) {
    throw Exceptions::MessageException("Failed to send message: " +
                                       std::string(std::strerror(errno)));
  }
}

std::optional<std::string> MessageQueue::receive() {
  if (!m_isOpen || m_descriptor == -1) {
    throw Exceptions::MessageException("Message queue is not open");
  }

  char buffer[MAX_MESSAGE_SIZE];
  unsigned int priority;
  ssize_t bytesRead;

  bytesRead = mq_receive(m_descriptor, buffer, MAX_MESSAGE_SIZE, &priority);

  if (bytesRead == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return std::nullopt;
    }
    throw Exceptions::MessageException("Failed to receive message: " +
                                       std::string(std::strerror(errno)));
  }

  return std::string(buffer, bytesRead);
}

std::optional<std::string>
MessageQueue::timedReceive(std::chrono::milliseconds timeout) {
  if (!m_isOpen || m_descriptor == -1) {
    throw Exceptions::MessageException("Message queue is not open");
  }

  char buffer[MAX_MESSAGE_SIZE];
  unsigned int priority;

  struct timespec timeSpec;
  auto now = std::chrono::system_clock::now();
  auto deadline = now + timeout;
  std::time_t expirationTime = std::chrono::system_clock::to_time_t(deadline);
  auto remainingNanoseconds =
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          deadline.time_since_epoch() % std::chrono::seconds(1));

  timeSpec.tv_sec = expirationTime;
  timeSpec.tv_nsec = remainingNanoseconds.count();

  ssize_t bytesRead = mq_timedreceive(m_descriptor, buffer, MAX_MESSAGE_SIZE,
                                      &priority, &timeSpec);

  if (bytesRead == -1) {
    if (errno == ETIMEDOUT || errno == EAGAIN) {
      return std::nullopt;
    }
    throw Exceptions::MessageException("Failed to receive message: " +
                                       std::string(std::strerror(errno)));
  }

  return std::string(buffer, bytesRead);
}

void MessageQueue::close() {
  if (m_isOpen && m_descriptor != -1) {
    mq_close(m_descriptor);
    if (m_isCreator) {
      mq_unlink(m_name.c_str());
    }
    m_descriptor = -1;
    m_isOpen = false;
  }
}
} // namespace Plazza::Communication
