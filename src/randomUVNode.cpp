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
MObject randomUVNode::stTime;
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
		time = nAttr.create("time", "t", MFnNumericData::kInt, 0, &status);
		status = nAttr.setWritable(true);
		status = nAttr.setStorable(true);
		status = nAttr.setKeyable(true);

	}

    {
        MFnNumericAttribute nAttr;
        stTime = nAttr.create("startTime", "stT", MFnNumericData::kInt, 0, &status);
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
    addAttribute(stTime);
	addAttribute(frequency);
	addAttribute(outMesh);

	attributeAffects(inMesh, outMesh);
    attributeAffects(time, outMesh);
    attributeAffects(stTime, outMesh);
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
    MDataHandle stTimeHandle = data.inputValue(stTime, &returnStatus);
	MDataHandle freqHandle = data.inputValue(frequency, &returnStatus);

	int getT = timeHandle.asInt();
    int getST = stTimeHandle.asInt();
	int freqT = freqHandle.asInt();

	unsigned int length = inMeshFn.numPolygons();

	MDataHandle outMeshHandle = data.outputValue(outMesh, &returnStatus);
	MObject outMeshObj = outMeshHandle.asMesh();
    
    MFnMesh createFn;
    // 初始形态
    if (getT <= getST) {
        changeTime(getST);
        createFn.copy(inMeshObj, outMeshData);
        outMeshHandle.set(outMeshData);
        outMeshHandle.setClean();
        return returnStatus;
    }
    // 不需要变化的情况就跳过
    if (getT < freqT + changeTime()) {
        outMeshHandle.setClean();
    	return returnStatus;
    }

	//if (std::fmod(getT, freqT) != 0.0) {
	//	outMeshHandle.setClean();
	//	return returnStatus;
	//}
	// 需要结算的情况
    changeTime(getT);
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
		MPoint mp = vertexArray[faceId * 4 + 0];
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

int randomUVNode::changeTime()
{
    return randomUVNode::getChangeTime;
}

void randomUVNode::changeTime(const int time)
{
    randomUVNode::getChangeTime = time;
}
