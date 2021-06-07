
// GameV5View.h: CGameV5View 클래스의 인터페이스
//

#pragma once


// 내가 쓸 클래스들
#include "Ball.h"
#include "Brick.h"
#include "DropBall.h"

// 내가 쓸 함수들
#include "MyMath.h"

#pragma comment(lib, "winmm")
#include <mmsystem.h>

enum State{BEGIN, RUNNING, END};

class CGameV5View : public CView
{
protected: // serialization에서만 만들어집니다.
	CGameV5View() noexcept;
	DECLARE_DYNCREATE(CGameV5View)

// 특성입니다.
public:
	CGameV5Doc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CGameV5View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void UpdateGameData(void);
	void GenerateNewLine(unsigned char);

	bool m_bIsBallMoving;
	bool m_bIsDBallMoving;
	int m_iState;

	CRect gameFrame;
	CRect startBtn;
	CRect restartBtn;
	CRect guideFrame;
	


	CArray<Brick, Brick&> brickList;
	CArray<Ball, Ball&> ballList;
	CArray<DropBall, DropBall&> dropBallList;

	CPoint m_ptMouseAim;
	CPoint m_ptAimLineEnd;
	CPoint m_ptStart;

	bool m_bIsAiming;
	double m_dAimVector[2];
	int m_iBallCnt;
	bool m_bIsAllBallShooted;
	double m_dBallSpeed;
	int m_iStage;
	double m_dPointOnRect[2];
	double m_dReflectAngle;
	int m_iDeadBallCount;
	bool m_bSettingNextStage;
	int m_iDropBallCnt;
	int m_iMovinBallCnt;
};

#ifndef _DEBUG  // GameV5View.cpp의 디버그 버전
inline CGameV5Doc* CGameV5View::GetDocument() const
   { return reinterpret_cast<CGameV5Doc*>(m_pDocument); }
#endif

