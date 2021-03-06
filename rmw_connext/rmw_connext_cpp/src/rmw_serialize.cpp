// Copyright 2014-2018 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "rmw/error_handling.h"
#include "rmw/rmw.h"

#include "./type_support_common.hpp"

// include patched generated code from the build folder
#include "connext_static_serialized_dataSupport.h"

extern "C"
{
rmw_ret_t
rmw_serialize(
  const void * ros_message,
  const rosidl_message_type_support_t * type_support,
  rmw_serialized_message_t * serialized_message)
{
  RMW_CONNEXT_EXTRACT_MESSAGE_TYPESUPPORT(type_support, ts, RMW_RET_ERROR)

  const message_type_support_callbacks_t * callbacks =
    static_cast<const message_type_support_callbacks_t *>(ts->data);
  if (!callbacks) {
    RMW_SET_ERROR_MSG("callbacks handle is null");
    return RMW_RET_ERROR;
  }

  ConnextStaticCDRStream cdr_stream;
  cdr_stream.buffer = serialized_message->buffer;
  cdr_stream.buffer_length = serialized_message->buffer_length;
  cdr_stream.buffer_capacity = serialized_message->buffer_capacity;
  cdr_stream.allocator = serialized_message->allocator;

  if (!callbacks->to_cdr_stream(ros_message, &cdr_stream)) {
    RMW_SET_ERROR_MSG("failed to convert ros_message to cdr stream");
    return RMW_RET_ERROR;
  }
  // reassgin buffer because it might have been resized
  serialized_message->buffer = cdr_stream.buffer;
  serialized_message->buffer_length = cdr_stream.buffer_length;
  serialized_message->buffer_capacity = cdr_stream.buffer_capacity;
  return RMW_RET_OK;
}

rmw_ret_t
rmw_deserialize(
  const rmw_serialized_message_t * serialized_message,
  const rosidl_message_type_support_t * type_support,
  void * ros_message)
{
  RMW_CONNEXT_EXTRACT_MESSAGE_TYPESUPPORT(type_support, ts, RMW_RET_ERROR)

  const message_type_support_callbacks_t * callbacks =
    static_cast<const message_type_support_callbacks_t *>(ts->data);
  if (!callbacks) {
    RMW_SET_ERROR_MSG("callbacks handle is null");
    return RMW_RET_ERROR;
  }

  ConnextStaticCDRStream cdr_stream;
  cdr_stream.buffer = serialized_message->buffer;
  cdr_stream.buffer_length = serialized_message->buffer_length;
  cdr_stream.buffer_capacity = serialized_message->buffer_capacity;
  cdr_stream.allocator = serialized_message->allocator;
  // convert the cdr stream to the message
  if (!callbacks->to_message(&cdr_stream, ros_message)) {
    RMW_SET_ERROR_MSG("can't convert cdr stream to ros message");
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}
}  // extern "C"
