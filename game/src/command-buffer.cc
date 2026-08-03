#include "command-buffer.hh"
#include "context.hh"

static_assert(std::is_trivially_copyable_v<Command>);
static_assert(std::is_trivially_copyable_v<CommandReply>);

CommandBuffer::CommandBuffer() : batches_(), generation_{0u} {}

Handle<Command> CommandBuffer::post(Command cmd)
{
  Batch& batch = CommandBuffer::get_batch(generation_);
  if (batch.size() >= max_command_count) return Handle<Command>::null();
  return {batch.append(cmd), generation_};
}

std::optional<CommandReply> CommandBuffer::poll(Handle<Command> handle) const
{
  PRECONDITION(handle, "polled a null handle");
  PRECONDITION(handle.generation <= generation_, "handle from a future generation");
  if (generation_ - handle.generation >= max_batch_count) return std::nullopt;
  Batch const& batch = get_batch(handle.generation);
  PRECONDITION(handle.index < batch.size(), "handle was never issued in its generation");
  CommandRequest const& request = batch[handle.index];
  if (request.is_resolved()) return request.as_reply();
  return CommandReply{CommandStatus::Pending};
}

void CommandBuffer::dispatch(Context ctx)
{
  for (CommandRequest& request : get_batch(generation_).view()) {
    DEBUG_ASSERT(!request.is_resolved(), "drained a batch twice");
    request.put(submit_command(ctx, request.as_command()));
  }
  get_batch(++generation_).reset();
}
