#pragma once
//#ifndef _randomUVNode__
//#define _randomUVNode__
#include <maya/MPxNode.h>

class randomUVNode : public MPxNode
{
public:
	randomUVNode() {};
	~randomUVNode() override {};
	virtual MStatus     compute(const MPlug& plug, MDataBlock& data) override;
	static  void*       creator() { return new randomUVNode; };
	static  MStatus     initialize();
public:

	static  MObject     inMesh;
	static  MObject		time;
    static MObject      stTime;
	static  MObject		frequency;
	static  MObject     outMesh;
	static const		MTypeId		id;		

private:
	MStatus getOutMesh(const MPlug& plug, MDataBlock& data);

    int getChangeTime = 0;
    int changeTime();
    void changeTime(const int time);
};
//#endif
