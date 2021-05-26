module;
#include <thread>
export module Vitro.Task.TaskSystem;

import Vitro.Core.Array;

export class TaskSystem
{
private:
	Array<std::jthread> threads;
};
