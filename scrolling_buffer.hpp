#ifndef OMEGA_SCROLLING_BUFFER_HPP_
#define OMEGA_SCROLLING_BUFFER_HPP_

#include <deque>
#include <queue>
#include <string>
#include <cstdint>

class scrolling_buffer
{
public:
  struct queued_message
  {
    std::string message;
    bool force_break;
  };
  scrolling_buffer(uint16_t width = 80, uint16_t length = 64);
  void receive(const std::string &message, bool force_break = false);
  void append(const std::string &message, bool pad = true, bool force_break = false);
  void replace_last(const std::string &message);
  void resize(uint16_t width, uint16_t length);
  void clear();
  const std::deque<std::string> &get_message_history(bool update = true);
  uint16_t                       get_width() const;
  uint16_t                       get_length() const;

private:
  uint16_t                width;
  uint16_t                length;
  void                    process_queue();
  std::deque<std::string> message_history;
  std::queue<queued_message> message_queue;
};

#endif
