#pragma once

#define NOTIFY_LISTENERS(call) for(iterator it=begin(); it!=end(); it++) (*it)->call

#define INCLUDE_NAMESPACE(label) using namespace label;
#define BEGIN_NAMESPACE(label) namespace label {
#define END_NAMESPACE(label) }
