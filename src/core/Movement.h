#pragma once
#ifndef _MOVEMENT_H_BY_DP_INCLUDED_
#define _MOVEMENT_H_BY_DP_INCLUDED_

#include "Global.h"

#include "Mesh.h"
#include "Quaternion.h"
#include "Transform.h"

#include "DPFileInfo.h"

#include "BaseObject.h"

#include "Mesh.h"


#include <set>

class CAnnotation;

class DPVISION_EXPORT CMovement : public CObject
{
public:
	class FrameVal {
	public:
		FrameVal() : label("frame"),msec(0),t(CTransform()) {};
		FrameVal(int _msec, CTransform _t) : label("frame"), msec(_msec), t(_t) {};
		FrameVal(int _msec, QString _l, CTransform _t) : msec(_msec), label(_l), t(_t) {};

		void setLabel(QString _l) { label = _l; };
		QString getLabel() { return label; };

		QString label;
		int msec;
		CTransform t;
	};

	typedef QList<FrameVal> SeqList;
	typedef QListIterator<FrameVal> SeqListIterator;

protected:
	bool m_isPlaying = false;
	int m_currentKey = 0;
	QTimer m_animationTimer;

public:
	SeqList m_seqlist;

	CMovement();
	CMovement(const CMovement& m);
	CMovement(const SeqList& s);

	virtual ~CMovement() { m_seqlist.clear(); m_animationTimer.stop(); }

	inline int type() override { return CObject::Type::MOVEMENT; };
	virtual std::shared_ptr<CBaseObject> getCopy() override {
		auto obj = std::make_shared<CMovement>(*this);
		updateChildrenParentPointers(obj);
		return obj;
	};

	void info(std::wstring i[4]) override;
	std::wstring infoRow() override;

	virtual bool hasTransformation() override { return true; };
	virtual Eigen::Matrix4d getTransformationMatrix() override { return this->currentFrame().t.toEigenMatrix4d(); };

	void startPlaying();
	void stopPlaying();
	inline bool isPlaying() { return m_isPlaying; }

	void setKey(int k);
	int increaseKey(bool _loop = false);
	inline int currentKey() { return m_currentKey; }

	inline int size() const { return m_seqlist.size(); } // liczba klatek
	inline FrameVal& currentFrame() { return m_seqlist[m_currentKey]; } // bie¿¹ca klatka
	FrameVal& frame(int k); // klatka o wskazanym numerze
private:
	void setTimer(); // wo³aæ tylko w konstruktorze
	void onTimeout();

public:
	// tu inkrenentacja licznika klatek;
	void renderFrame(); // rysowanie bie¿¹cej klatki
	void renderRotationAxe(CTransform t, CTransform prev = CTransform());
};


#endif /* _MOVEMENT_H_BY_DP_INCLUDED_ */