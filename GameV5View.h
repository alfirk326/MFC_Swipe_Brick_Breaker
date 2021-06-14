
// GameV5View.h: CGameV5View 클래스의 인터페이스
//

#pragma once


// 내가 쓸 클래스들
#include "Ball.h"
#include "Brick.h"
#include "DropBall.h"
#include "RegRankDlg.h"
#include "ShowRankDlg.h"

// 내가 쓸 함수들
#include "MyMath.h"

#pragma comment(lib, "winmm")
#include <mmsystem.h>


// 세가지 상태로 분류한다.
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

public:
	void UpdateGameData(void);					   // 게임 정보 업데이트 하는 루틴
	void GenerateNewLine(unsigned char);		   // 새로운 벽돌 라인 배치하는 루틴
												  
	CRect m_rectGameFrame;	
	CRect m_rectStartBtn;	
	CRect m_rectRestartBtn;	
	CRect m_rectRegRankBtn;	
	CRect m_rectShowRankBtn;
	CRect m_rectGuideFrame;	

	unsigned int m_uiTimeWhenPlayed;
												   
	CArray<Brick, Brick&> brickList;			   //
	CArray<Ball, Ball&> ballList;				   //
	CArray<DropBall, DropBall&> dropBallList;	   //
	
	CPoint m_ptMouseAim;						   //
	CPoint m_ptAimLineEnd;						   //
	CPoint m_ptStart;							   //
												   //
	bool m_bIsAiming;							   //
	bool m_bIsAllBallShooted;					   //
	bool m_bIsBallMoving; 						   // 공이 움직이는지
	bool m_bIsDBallMoving;						   // 드랍볼이 움직이는지
	bool m_bIsRegistered;						   // 점수가 등록 되었는지
	bool m_bSettingNextStage;					   //
	double m_dAimVector[2];						   //
	double m_dPointOnRect[2];					   //
	double m_dBallSpeed;						   //
	double m_dReflectAngle;						   //
	int m_iState;								   // 게임의 상태
	int m_iBallCnt;					   
	int m_iDeadBallCount;				
	int m_iDropBallCnt;				   
	int m_iMovinBallCnt;		
	unsigned int m_uiStage;

	ShowRankDlg m_dlgShowRank;
	RegRankDlg m_dlgRegRank;
};

#ifndef _DEBUG  // GameV5View.cpp의 디버그 버전
inline CGameV5Doc* CGameV5View::GetDocument() const
   { return reinterpret_cast<CGameV5Doc*>(m_pDocument); }
#endif

