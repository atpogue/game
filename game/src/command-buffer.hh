#pragma once
#include "command.hh"
#include "core/panic.hh"
#include "core/types.hh"
#include <array>
#include <optional>
#include <span>

struct CommandRequest
{
  inline void put(Command const& cmd)
  {
    kind_           = Kind::Command;
    detail_.command = cmd;
  }

  inline void put(CommandReply const& reply)
  {
    kind_         = Kind::Reply;
    detail_.reply = reply;
  }

  [[nodiscard]] inline Command const& as_command() const
  {
    DEBUG_ASSERT(kind_ == Kind::Command);
    return detail_.command;
  }

  [[nodiscard]] inline CommandReply const& as_reply() const
  {
    DEBUG_ASSERT(kind_ == Kind::Reply);
    return detail_.reply;
  }

  [[nodiscard]] inline bool is_resolved() const { return kind_ == Kind::Reply; }

  [[nodiscard]] inline explicit operator bool() const { return is_resolved(); }

private:
  union Detail
  {
    Command      command;
    CommandReply reply;
  };

  enum class Kind : bool { Command, Reply };

  Kind   kind_   = Kind::Command;
  Detail detail_ = { .command = {} };
};

struct CommandBuffer
{
  static constexpr u32 max_batch_count   = 2;
  static constexpr u32 max_command_count = 256;

  CommandBuffer();

  CommandBuffer(CommandBuffer&&) noexcept            = default;
  CommandBuffer& operator=(CommandBuffer&&) noexcept = default;
  CommandBuffer& operator=(CommandBuffer const&)     = delete;
  ~CommandBuffer() noexcept                          = default;

  Handle<Command> post(Command cmd);

  [[nodiscard]] std::optional<CommandReply> poll(Handle<Command> handle) const;

  void dispatch(Context ctx);

  [[nodiscard]] u32 count() const { return get_batch(generation_).size(); }

  [[nodiscard]] u32 generation() const { return generation_; }

  [[nodiscard]] std::span<CommandRequest const> view() const
  {
    return get_batch(generation_).view();
  }

  [[nodiscard]] CommandBuffer copy() const { return *this; }

private:
  CommandBuffer(CommandBuffer const&) = default;

  // placeholder name, find accurate name
  struct Batch
  {
    [[nodiscard]] CommandRequest const& operator[](u32 idx) const
    {
      DEBUG_ASSERT(idx < size_);
      return requests_[idx];
    }

    // Assumes: the batch is not full
    u32 append(Command const& cmd)
    {
      DEBUG_ASSERT(size_ < max_command_count);
      requests_[size_].put(cmd);
      return size_++;
    }

    [[nodiscard]] std::span<CommandRequest> view() { return { requests_.data(), size_ }; }

    [[nodiscard]] std::span<CommandRequest const> view() const
    {
      return { requests_.data(), size_ };
    }

    [[nodiscard]] u32 size() const { return size_; }

    void reset() { size_ = 0u; }

  private:
    std::array<CommandRequest, max_command_count> requests_;
    u32                                           size_ = 0u;
  };

  [[nodiscard]] Batch& get_batch(u32 gen) { return batches_[gen % max_batch_count]; }

  [[nodiscard]] Batch const& get_batch(u32 gen) const { return batches_[gen % max_batch_count]; }

  std::array<Batch, max_batch_count> batches_;
  u32                                generation_;
};
