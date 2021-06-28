#pragma once

#include <set>
#include <vector>
#include <algorithm>
#include <set>
#include "core/custom_struct.h"

namespace asyncflow
{
	namespace core
	{
		class Node;
		class AsyncManager
		{
		public:
			AsyncManager()	{}
			void Step();
			void AddNode(Node* node);			
			void RemoveNode(Node* node);
			bool IsNodeWaiting(Node* node);
			void ActivateNode(Node* node, bool skip_flag = true);
#ifdef FLOWCHART_DEBUG
			// break_point command when debug	
			void ContinueBreakpoint(Chart* chart);
			void ContinueBreakpoint(NodeData* node_data);
#endif
		private:
			NodeList waiting_nodes_;				//waiting for result
			std::vector<Node*> activate_nodes;		//the result has been returned, waiting for execution
			std::set<Node*> nodes_set_;				//facilitate search
			
		};
	}
}