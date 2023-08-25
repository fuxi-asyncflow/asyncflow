#pragma once

#include <vector>
#include <unordered_set>
#include "core/custom_struct.h"

namespace asyncflow
{
	namespace core
	{
		class Node;
		class Chart;
		class NodeData;

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
			_NodeList waiting_nodes_;				//waiting for result
			_NodeList activate_nodes_;		//the result has been returned, waiting for execution
			std::unordered_set<Node*> nodes_set_;	//facilitate search
			
		};
	}
}