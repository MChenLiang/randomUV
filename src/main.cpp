#include "randomUVNode.h"

#include <maya/MFnPlugin.h>

MStatus initializePlugin(MObject obj)
{
	MStatus result;
	MFnPlugin plugin(obj, "MCL", "1.0", "Any");

	// ×¢²áslidingOnMesh½Úµã
	result = plugin.registerNode("randomUVNode",
		randomUVNode::id,
		randomUVNode::creator,
		randomUVNode::initialize);
	if (!result) {
		result.perror("registerNode error: randomUVNode");
		return result;
	}
	return result;
}
MStatus uninitializePlugin(MObject obj)
{
	MStatus result;
	MFnPlugin plugin(obj);
	result = plugin.deregisterNode(randomUVNode::id);
	if (!result) {
		result.perror("deregisterNode error: randomUVNode");
		return result;
	}

	return result;
}