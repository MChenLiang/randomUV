#include "randomUVNode.h"
#include <maya/MGlobal.h>
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

    MString pyCmds;
    pyCmds += "from randomUV import randomUVMenu\n";
    pyCmds += "from imp import reload\n";
    pyCmds += "reload(randomUVMenu)\n";
    pyCmds += "randomUVMenu.add_menu()";

    MGlobal::executePythonCommand(pyCmds);

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

    MString pyCmds;
    pyCmds += "from randomUV import randomUVMenu\n";
    pyCmds += "from imp import reload\n";
    pyCmds += "reload(randomUVMenu)\n";
    pyCmds += "randomUVMenu.delete_menu()";

    MGlobal::executePythonCommand(pyCmds);

	return result;
}