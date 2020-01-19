#include "randomUVNode.h"
#include "MacroDefinition.h"
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MTime.h>
#include <maya/MGlobal.h>

#include <math.h>		// std::fmod
#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector

MObject randomUVNode::inMesh;
MObject randomUVNode::time;
MObject randomUVNode::frequency;
MObject randomUVNode::outMesh;

const		MTypeId     randomUVNode::id(0x0012f5d0);

int myrandom(int i) { return std::rand() % i; }

MStatus randomUVNode::initialize()
{
	MStatus status = MS::kSuccess;

	{
		MFnTypedAttribute   typedAttr;
		inMesh = typedAttr.create("inMesh", "inm", MFnData::kMesh, &status);
	}

	{
		MFnNumericAttribute nAttr;
		time = nAttr.create("time", "tm", MFnNumericData::kInt, 0, &status);
		status = nAttr.setWritable(true);
		status = nAttr.setStorable(true);
		status = nAttr.setKeyable(true);

	}

	{
		MFnNumericAttribute nAttr;
		frequency = nAttr.create("frequency", "f", MFnNumericData::kInt, 0, &status);
		status = nAttr.setWritable(true);
		status = nAttr.setStorable(true);
		status = nAttr.setKeyable(true);
		status = nAttr.setMin(0);
		status = nAttr.setDefault(0);
	}

	{
		MFnTypedAttribute   typedAttr;
		MFnMeshData fnMeshData;
		MObject meshDefaultObject = fnMeshData.create(&status);
		outMesh = typedAttr.create("outMesh", "outm", MFnData::kMesh, meshDefaultObject);
		status = typedAttr.setWritable(false);
		status = typedAttr.setStorable(false);
		status = typedAttr.setKeyable(false);
	}

	addAttribute(inMesh);
	addAttribute(time);
	addAttribute(frequency);
	addAttribute(outMesh);

	attributeAffects(inMesh, outMesh);
	attributeAffects(time, outMesh);
	attributeAffects(frequency, outMesh);

	return status;

}



MStatus randomUVNode::compute(const MPlug & plug, MDataBlock & data)
{
	MStatus returnStatus = MS::kSuccess;
	if (plug.attribute() == outMesh) {
		getOutMesh(plug, data);
		data.setClean(plug);

	}
	else {
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}

MStatus randomUVNode::getOutMesh(const MPlug & plug, MDataBlock & data)
{
	MStatus returnStatus = MS::kSuccess;

	MFnMeshData inCreator;
	MObject outMeshData = inCreator.create(&returnStatus);
	MDataHandle inMeshHandle = data.inputValue(inMesh, &returnStatus);

	MObject inMeshObj = inMeshHandle.asMesh();
	MFnMesh inMeshFn(inMeshObj);

	MDataHandle timeHandle = data.inputValue(time, &returnStatus);
	MDataHandle freqHandle = data.inputValue(frequency, &returnStatus);

	unsigned int getT = timeHandle.asInt();
	unsigned int freqT = freqHandle.asInt();

	unsigned int length = inMeshFn.numPolygons();

	MDataHandle outMeshHandle = data.outputValue(outMesh, &returnStatus);
	MObject outMeshObj = outMeshHandle.asMesh();
	if (!outMeshObj.isNull() && std::fmod(getT, freqT) != 0.0) {
		outMeshHandle.setClean();
		return returnStatus;
	}
	MFnMesh createFn;
	createFn.copy(inMeshObj, outMeshData);
	MFnMesh outMeshFn(outMeshData);

	unsigned i, j;
	std::vector<int> faceIds;
	for (i = 0; i < length; ++i) faceIds.push_back(i);
	std::random_shuffle(faceIds.begin(), faceIds.end(), myrandom);

	MPointArray vertexArray, outVtxArray;
	inMeshFn.getPoints(vertexArray, MSpace::kWorld);

	outVtxArray.setLength(vertexArray.length());
	j = 0;
	for (std::vector<int>::iterator it = faceIds.begin(); it != faceIds.end(); ++it) {
		unsigned int faceId = *it;
		//MGlobal::displayInfo(MString("face id") + " -- " + faceId);
		MPoint mp = vertexArray[faceId * 4 + 0];
		//MGlobal::displayInfo(MString("temp point") + " -- " + mp.x + mp.y + mp.z);
		outVtxArray[j * 4 + 0] = vertexArray[faceId * 4 + 0];
		outVtxArray[j * 4 + 1] = vertexArray[faceId * 4 + 1];
		outVtxArray[j * 4 + 2] = vertexArray[faceId * 4 + 2];
		outVtxArray[j * 4 + 3] = vertexArray[faceId * 4 + 3];
		j++;

	}
	outMeshFn.setPoints(outVtxArray, MSpace::kWorld);

	outMeshFn.updateSurface();
	outMeshHandle.set(outMeshData);
	outMeshHandle.setClean();
	
	return returnStatus;
}
