module;
#include <any>
#include <deque>
export module vt.Graphics.DeletionQueue;

namespace vt
{
	export class DeletionQueue
	{
	public:
		void submit(std::any resource)
		{
			resources.emplace_back(std::move(resource));
		}

		void delete_all()
		{
			resources.clear();
		}

	private:
		std::deque<std::any> resources;
	};
}
