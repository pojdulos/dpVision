#include "Movement.h"
#include "Movement.h"

#include "Annotation.h"
#include "AnnotationPoint.h"

#include "PMFactory.h"
#include "Utilities.h"

#include "AP.h"

#include "MainApplication.h"

#include "GLViewer.h"
#include "Parser.h"

#include "../renderers/IMovementRenderer.h"

CMovement::CMovement() : CObject(nullptr), m_isPlaying(false), m_currentKey(0)
{
	setLabel("animation");
	bDrawBB = false;
	setTimer();

	renderer_ = std::make_shared<IMovementRenderer>();
}

CMovement::CMovement(const CMovement& m) : CObject(m), m_seqlist(m.m_seqlist), m_isPlaying(false), m_currentKey(0)
{
	bDrawBB = false;
	setTimer();

	renderer_ = std::make_shared<IMovementRenderer>();
}

CMovement::CMovement(const SeqList& s) : CObject(nullptr), m_seqlist(s), m_isPlaying(false), m_currentKey(0)
{
	setLabel("animation");
	bDrawBB = false;
	setTimer();

	renderer_ = std::make_shared<IMovementRenderer>();
}


CMovement::FrameVal& CMovement::frame(int k)
{
	if (k < 0) return m_seqlist.front();
	else if (k >= m_seqlist.size()) return m_seqlist.back();

	return m_seqlist[k];
}


void CMovement::renderRotationAxe(CTransform curr, CTransform prev)
{
	CQuaternion prevQ = prev.rotation();
	CQuaternion currQ = curr.rotation();

	CQuaternion relQ = prevQ.inverse() * currQ;

	CVector3d prevT = prev.translation();
	CVector3d currT = curr.translation();

	CVector3d relT = currT - prevT;

	Eigen::AngleAxisd absAA(currQ.toRotationMatrix());
	Eigen::Vector3d absAxis = absAA.axis();
	double absAngle = absAA.angle(); // K�t w radianach
	double absLen = 200.0;// 1000.0 * absAngle;

	Eigen::AngleAxisd relAA(relQ.toRotationMatrix());
	Eigen::Vector3d relAxis = relAA.axis();
	double relAngle = relAA.angle(); // K�t w radianach
	double relLen = 200.0;// 10000.0 * relAngle;

	qInfo() << "-------------------------------------------------------------" << Qt::endl;
	qInfo() << "ABS angle: " << rad2deg(absAngle) << "� translation: " << currT.length() << "mm" << Qt::endl;
	qInfo() << "REL angle: " << rad2deg(relAngle) << "� translation: " << relT.length() << "mm" << Qt::endl;

	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);


	glColor3f(1.0, 0.0, 1.0);

	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex3f(absAxis.x() * -absLen, absAxis.y() * -absLen, absAxis.z() * -absLen);
	glVertex3f(absAxis.x() * absLen, absAxis.y() * absLen, absAxis.z() * absLen);
	glEnd();

	glColor3f(1.0, 0.7, 0.0);

	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex3f(relAxis.x() * -relLen, relAxis.y() * -relLen, relAxis.z() * -relLen);
	glVertex3f(relAxis.x() * relLen, relAxis.y() * relLen, relAxis.z() * relLen);
	glEnd();


	glPopAttrib();
	glPopMatrix();
}

void CMovement::renderFrame()
{
	FrameVal frame = m_seqlist[m_currentKey];
	//for (FrameVal frame : m_seqlist)
	{
		if (m_currentKey>0) 
			renderRotationAxe(frame.t, m_seqlist[m_currentKey-1].t);
		else
			renderRotationAxe(frame.t);

		glPushMatrix();
		frame.t.render();


		for (const auto& itd : m_data)
		{
			switch (itd.second->type())
			{
			case CObject::MODEL:
				itd.second->render();
				break;
			default:
				glLoadName(m_Id);
				glPushName(m_Id);

				itd.second->render();

				glPopName();
				glLoadName(0);
				break;
			}
		}

		for (const auto& it : m_annotations)
		{
			it.second->render();
		}

		glPopMatrix();

	}
}


void CMovement::setTimer()
{
	m_animationTimer.setSingleShot(true);
	QObject::connect(&m_animationTimer, &QTimer::timeout, [&]() { onTimeout(); });
}

#include "UI.h"

void CMovement::onTimeout()
{
	if (m_isPlaying) {
		m_currentKey = (m_currentKey + 1) % m_seqlist.size();

		UI::DOCK::PROPERTIES::updateProperties();
		UI::updateAllViews();
		
		m_animationTimer.start(m_seqlist[m_currentKey].msec);
	}
}

void CMovement::startPlaying()
{
	m_animationTimer.start(m_seqlist[m_currentKey].msec);
	m_isPlaying = true;
}

void CMovement::stopPlaying()
{
	m_isPlaying = false;
	m_animationTimer.stop();
}

void CMovement::setKey(int k) {
	if (k < 0) m_currentKey = 0;
	else if (k >= m_seqlist.size()) m_currentKey = m_seqlist.size() - 1;
	else m_currentKey = k;
}

int CMovement::increaseKey(bool _loop)
{
	m_currentKey++;

	if (_loop)
		return (m_currentKey = m_currentKey % m_seqlist.size());
	else if (m_currentKey >= m_seqlist.size())
		return (m_currentKey = m_seqlist.size() - 1);
	
	return m_currentKey;
}

std::wstring CMovement::infoRow()
{
	//size_t n = m_data.size();
	//size_t np = m_annotations.size();
	std::wstring ret = L"Movement (id:"+ std::to_wstring(m_Id) + L")";
	
	//if ( n+np == 0 )
	//	ret += L"not any children";
	//if (n > 0)
	//{
	//	ret += std::to_wstring(n) + ((n < 2) ? L" data child " : L" data children ");
	//	if (np > 0) ret += L" and ";
	//}
	//
	//if (np > 0)
	//{
	//	ret += std::to_wstring(np) + ((np < 2) ? L" annotation " : L" annotations ");
	//}
	
	return ret;
}




void CMovement::info(std::wstring i[4])
{
	i[0] = m_label.toStdWString();
	i[1] = std::to_wstring( m_Id );
	i[2] = L"Movement";
	i[3] = infoRow();
}

