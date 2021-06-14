
// GameV5View.cpp: CGameV5View 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "GameV5.h"
#endif

#include "GameV5Doc.h"
#include "GameV5View.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TICKRATE 80

// CGameV5View

IMPLEMENT_DYNCREATE(CGameV5View, CView)

BEGIN_MESSAGE_MAP(CGameV5View, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CGameV5View 생성/소멸

CGameV5View::CGameV5View() noexcept
	: m_ptStart(260, 600)
	, m_ptAimLineEnd(260, 600)
	, m_rectGameFrame(10, 60, 510, 610)
	, m_rectGuideFrame(60, 250, 470, 430)
	, m_rectStartBtn(100, 500, 200, 550)
	, m_rectRestartBtn(210, 350, 310, 400)
	, m_rectRegRankBtn(210, 420, 310, 470)
	, m_rectShowRankBtn(300, 500, 400, 550)
	, m_uiTimeWhenPlayed(0)
	, m_iState(BEGIN)
	, m_bIsBallMoving(false)
	, m_bIsDBallMoving(false)
	, m_bIsAiming(false)
	, m_bIsAllBallShooted(true)
	, m_bIsRegistered(false)
	, m_dAimVector{ 0.0, 0.0 }
	, m_dPointOnRect{ 0.0, 0.0 }
	, m_dReflectAngle(0.0)
	, m_dBallSpeed(8.0)
	, m_uiStage(0)
	, m_iMovinBallCnt(0)
	, m_iBallCnt(0)
	, m_bSettingNextStage(false)
	, m_iDeadBallCount(0)
	, m_iDropBallCnt(0)
{
	// TODO: 여기에 생성 코드를 추가합니다.
}

CGameV5View::~CGameV5View()
{
}

BOOL CGameV5View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CGameV5View 그리기

void CGameV5View::OnDraw(CDC* pDC)
{
	CGameV5Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.

	CRect WndRect;
	CDC memDC;
	CBitmap myBitmap;
	CBitmap* pOldBitmap;
	CString str;
	CRect rect;

	GetClientRect(WndRect);
	
	memDC.CreateCompatibleDC(pDC); // 메모리 DC 생성
	myBitmap.CreateCompatibleBitmap(pDC, WndRect.Width(), WndRect.Height());  // pDC와 호환되는 윈도우 크기의 비트맵을 만든다.
	pOldBitmap = memDC.SelectObject(&myBitmap);
	memDC.PatBlt(0, 0, WndRect.Width(), WndRect.Height(), WHITENESS);		// 이전에 그린 그림을 흰색으로 덮는다.

	CPen* pOldPen;
	CBrush* pOldBrush;
	CFont* pOldFont;

	SetBkMode(memDC, TRANSPARENT);

	CFont boldFont;
	boldFont.CreateFont(
		25,									//글자높이
		10,									//글자너비
		0,									//출력각도
		0,									//기준 선에서의각도
		FW_HEAVY,							//글자굵기
		FALSE,								//Italic 적용여부
		FALSE,								//밑줄적용여부
		FALSE,								//취소선적용여부
		DEFAULT_CHARSET,					//문자셋종류
		OUT_DEFAULT_PRECIS,					//출력정밀도
		CLIP_DEFAULT_PRECIS,				//클리핑정밀도
		DEFAULT_QUALITY,					//출력문자품질
		DEFAULT_PITCH,						//글꼴Pitch
		_T("고딕")							//글꼴
	);

	CBrush brush;
	brush.CreateStockObject(NULL_BRUSH);
	pOldBrush = memDC.SelectObject(&brush);
	memDC.Rectangle(m_rectGameFrame);
	memDC.SelectObject(pOldBrush);

	switch (m_iState) {
	case BEGIN: {

		CBitmap background;
		BITMAP bmpInfo;
		CDC memDC2;

		memDC2.CreateCompatibleDC(&memDC);
		background.LoadBitmapW(IDB_BITMAP1);
		background.GetBitmap(&bmpInfo);

		memDC2.SelectObject(background);
		memDC.BitBlt(110, 70, bmpInfo.bmWidth, bmpInfo.bmHeight, &memDC2, 0, 0, SRCCOPY);

		background.DeleteObject();
		ReleaseDC(&memDC2);
		DeleteDC(memDC2);

		pOldFont = memDC.SelectObject(&boldFont);
		memDC.Rectangle(m_rectStartBtn);
		memDC.DrawText(_T("게임시작"), &m_rectStartBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		memDC.Rectangle(m_rectShowRankBtn);
		memDC.DrawText(_T("랭크조회"), &m_rectShowRankBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		str.Format(_T("[How to 스와이프 벽돌깨기]\n\n- 스와이프로 공을 날려 벽돌을 파괴하세요.\n- 공이 부딪히면 벽돌의 내구도가 감소합니다.\n- 내구도가 0이 되면 벽돌이 파괴됩니다.\n- 녹색 원을 획득하면 공의 수가 늘어납니다.\n- 하단 라인까지 벽돌이 내려오면 게임오버 됩니다."));
		memDC.DrawText(str, &m_rectGuideFrame, NULL);
		memDC.SelectObject(pOldFont);

		break;
	}
	case RUNNING: {

		CBrush shadowBrush;
		shadowBrush.CreateSolidBrush(RGB(215, 206, 202));

		CBrush greenBrush;
		greenBrush.CreateSolidBrush(RGB(49, 194, 95));

		CBrush blueBrush;
		blueBrush.CreateSolidBrush(RGB(91, 166, 238));

		CPen transPen;
		transPen.CreatePen(PS_NULL, 0, RGB(0, 0, 0));

		CPen hlightPen;
		hlightPen.CreatePen(PS_SOLID, 6, RGB(49, 194, 95));

		CPen shadowPen;
		shadowPen.CreatePen(PS_SOLID, 6, RGB(215, 206, 202));

		// 그림자그리기
		pOldPen = memDC.SelectObject(&transPen);
		pOldBrush = memDC.SelectObject(&shadowBrush);

		for (int i = 0; i < brickList.GetCount(); i++) {
			const Brick& brick = brickList.GetAt(i);
			memDC.Rectangle(brick.x1 + 1 + 2, brick.y1 + 1 + 4, brick.x2 - 1 + 2, brick.y2 - 1 + 4);
		}

		for (int i = 0; i < dropBallList.GetCount(); i++) {
			const DropBall& dBall = dropBallList.GetAt(i);
			memDC.Ellipse((int)nearbyint(dBall.x1) + 2, (int)nearbyint(dBall.y1) + 4, (int)nearbyint(dBall.x2) + 2, (int)nearbyint(dBall.y2) + 4);

			if (dBall.isValid == true) {
				memDC.SelectStockObject(NULL_BRUSH);
				memDC.SelectObject(&shadowPen);
				memDC.Ellipse((int)nearbyint(dBall.x1) - 10 + 2, (int)nearbyint(dBall.y1) - 10 + 4, (int)nearbyint(dBall.x2) + 10 + 2, (int)nearbyint(dBall.y2) + 10 + 4);
				memDC.SelectObject(&transPen);
				memDC.SelectObject(&shadowBrush);
			}

		}

		for (int i = 0; i < ballList.GetCount(); i++) {
			const Ball& ball = ballList.GetAt(i);
			memDC.Ellipse((int)nearbyint(ball.x1) + 2, (int)nearbyint(ball.y1) + 4, (int)nearbyint(ball.x2) + 2, (int)nearbyint(ball.y2) + 4);
		}

		memDC.SelectObject(pOldPen);
		memDC.SelectObject(pOldBrush);	// 여기까지 그림자 그리기

		// 조준선 그리기
		if (m_bIsAiming == TRUE) {
			memDC.MoveTo(m_ptStart);
			memDC.LineTo(m_ptAimLineEnd);
		}

		// 벽돌그리기
		pOldBrush = memDC.GetCurrentBrush();
		pOldFont = memDC.SelectObject(&boldFont);
		memDC.SetTextColor(RGB(255, 255, 255));

		for (int i = 0; i < brickList.GetCount(); i++) {
			const Brick& brick = brickList.GetAt(i);
			CBrush cBrush;

			int life = brick.life;
			int red = Normalize(1, m_uiStage, life, 240, 255);
			int green = Normalize(1, m_uiStage, life, 155, 65);
			int blue = Normalize(1, m_uiStage, life, 110, 75);

			cBrush.CreateSolidBrush(RGB(red, green, blue));
			memDC.SelectObject(&cBrush);

			memDC.Rectangle(brick.x1 + 1, brick.y1  + 1, brick.x2 - 1, brick.y2 - 1);
			str.Format(_T("%d"), life);

			rect.SetRect(brick.x1, brick.y1, brick.x2, brick.y2);
			memDC.DrawText(str, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		memDC.SetTextColor(RGB(0, 0, 0));
		memDC.SelectObject(pOldFont);
		memDC.SelectObject(pOldBrush);	// 여기까지 벽돌 그리기


		// 드랍볼 그리기
		pOldBrush = memDC.SelectObject(&greenBrush);
		for (int i = 0; i < dropBallList.GetCount(); i++) {
			const DropBall& dBall = dropBallList.GetAt(i);
			memDC.Ellipse((int)nearbyint(dBall.x1), (int)nearbyint(dBall.y1), (int)nearbyint(dBall.x2), (int)nearbyint(dBall.y2));
			if (dBall.isValid == true) {
				pOldBrush = (CBrush*)memDC.SelectStockObject(NULL_BRUSH);
				pOldPen = memDC.SelectObject(&hlightPen);
				memDC.Ellipse((int)nearbyint(dBall.x1) - 10, (int)nearbyint(dBall.y1) - 10, (int)nearbyint(dBall.x2) + 10, (int)nearbyint(dBall.y2) + 10);
				memDC.SelectObject(pOldPen);
				memDC.SelectObject(pOldBrush);
			}
		}
		memDC.SelectObject(pOldBrush);

		// 공 그리기
		pOldBrush = memDC.SelectObject(&blueBrush);
		for (int i = 0; i < ballList.GetCount(); i++) {
			const Ball& ball = ballList.GetAt(i);
			memDC.Ellipse((int)nearbyint(ball.x1), (int)nearbyint(ball.y1), (int)nearbyint(ball.x2), (int)nearbyint(ball.y2));
		}
		memDC.SelectObject(pOldBrush);

		// x 공 갯수 텍스트 그리기
		pOldFont = memDC.SelectObject(&boldFont);
		memDC.SetTextColor(RGB(91, 166, 238));
		str.Format(_T("x%d"), m_iBallCnt - m_iMovinBallCnt);
		rect.SetRect(m_ptStart.x - 100, m_ptStart.y, m_ptStart.x + 100, m_ptStart.y + 50);
		memDC.DrawText(str, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		memDC.SelectObject(pOldFont);

		// 게임 프레임 오른쪽 위에 현재 점수 텍스트 그리기
		pOldFont = memDC.SelectObject(&boldFont);
		memDC.SetTextColor(RGB(0, 0, 0));
		memDC.SetTextAlign(TA_RIGHT);
		str.Format(_T("현재점수: %d"), m_uiStage);
		memDC.TextOutW(510, 30, str);
		memDC.SelectObject(pOldFont);

		break;
	}
	case END: {
		CFont gameOverFont;
		gameOverFont.CreateFont(
			60,									//글자높이
			30,									//글자너비
			0,									//출력각도
			0,									//기준 선에서의각도
			FW_HEAVY,							//글자굵기
			FALSE,								//Italic 적용여부
			FALSE,								//밑줄적용여부
			FALSE,								//취소선적용여부
			DEFAULT_CHARSET,					//문자셋종류
			OUT_DEFAULT_PRECIS,					//출력정밀도
			CLIP_DEFAULT_PRECIS,				//클리핑정밀도
			DEFAULT_QUALITY,					//출력문자품질
			DEFAULT_PITCH,						//글꼴Pitch
			_T("굴림")							//글꼴
		);
		pOldFont = memDC.SelectObject(&gameOverFont);
		memDC.SetTextAlign(TA_CENTER);
		memDC.TextOutW(260, 180, _T("게임오버"));
		memDC.SelectObject(&boldFont);
		memDC.SetTextColor(RGB(0, 0, 0));

		str.Format(_T("최종점수: %d"), m_uiStage);
		memDC.TextOutW(260, 250, str);
		memDC.SelectObject(pOldFont);
		memDC.SetTextAlign(TA_TOP | TA_LEFT);

		pOldFont = memDC.SelectObject(&boldFont);
		memDC.Rectangle(m_rectRestartBtn);
		memDC.DrawText(_T("다시하기"), &m_rectRestartBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		m_rectShowRankBtn.SetRect(210, 490, 310, 540);
		memDC.Rectangle(m_rectShowRankBtn);
		memDC.DrawText(_T("랭크조회"), &m_rectShowRankBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		memDC.Rectangle(m_rectRegRankBtn);
		memDC.DrawText(_T("점수등록"), &m_rectRegRankBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		break;
	}
	}

	pDC->BitBlt(0, 0, WndRect.Width(), WndRect.Height(), &memDC, 0, 0, SRCCOPY);
	pDC->SelectObject(pOldBitmap);
	myBitmap.DeleteObject();
	ReleaseDC(&memDC);
	DeleteDC(memDC);
}


// CGameV5View 인쇄

BOOL CGameV5View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CGameV5View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CGameV5View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CGameV5View 진단

#ifdef _DEBUG
void CGameV5View::AssertValid() const
{
	CView::AssertValid();
}

void CGameV5View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGameV5Doc* CGameV5View::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGameV5Doc)));
	return (CGameV5Doc*)m_pDocument;
}
#endif //_DEBUG


// CGameV5View 메시지 처리기


int CGameV5View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	// 타이머 동작 시작
	SetTimer(123, 1000/TICKRATE , NULL);

	return 0;
}


void CGameV5View::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	KillTimer(123);

	UpdateGameData();

	Invalidate();

	SetTimer(123, 1000/TICKRATE , NULL);

	CView::OnTimer(nIDEvent);
}


// 백그라운드가 지워졌다가 그려지는것을 방지한다.(깜빡임 방지)
BOOL CGameV5View::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return FALSE;
}


// 각종 계산을 여기서 한다.
void CGameV5View::UpdateGameData(void)
{
	switch (m_iState) {

	case BEGIN: {
		break;
	} // BEGIN

	case RUNNING: {
		if (m_bSettingNextStage == false) {

			// 이것은 모든 공이 멈춰있고 공을 조준할때의 상태이다.
			if (m_bIsBallMoving == false && m_bIsDBallMoving == false) {
				if (m_bIsAiming == true) {

					const double distance = GetDistance(m_ptMouseAim.x, m_ptMouseAim.y, m_ptStart.x, m_ptStart.y);

					m_dAimVector[0] = (m_ptStart.x - m_ptMouseAim.x) / distance;
					m_dAimVector[1] = (m_ptStart.y - m_ptMouseAim.y) / distance;

					m_ptAimLineEnd.SetPoint((int)nearbyint(m_ptStart.x + 300 * m_dAimVector[0]), 
						(int)nearbyint(m_ptStart.y + 300 * m_dAimVector[1]));
				}
			}

			// 공이 발사되는 순간부터 이 구문이 실행된다.
			else {

				// 드랍볼에 대한 처리
				for (int i = 0; i < dropBallList.GetCount(); i++) {
					DropBall& dBall = dropBallList.GetAt(i);
					if (dBall.isValid == false && dBall.isDead == false) {
						dBall.Move();
						m_bIsDBallMoving = true;
						if (dBall.y2 > m_rectGameFrame.BottomRight().y) {
							dBall.SetCenterPoint(dBall.x, m_rectGameFrame.BottomRight().y - Ball::radius);
							dBall.isDead = true;
							dBall.velocity[1] = 0.0;
							m_bIsDBallMoving = false;
						}
					}
				}

				// 발사한 공에 대한 처리
				for (int i = 0; i < ballList.GetCount(); i++) {

					// 볼 하나를 선택하고
					Ball& ball = ballList.GetAt(i);

					// 먼저 볼의 생사부터 확인한다.
					if (ball.isDead == false) {

						// 공이 움직이기 전에 공의 위치를 저장하고
						const double vecBuff[2] = { ball.x, ball.y }; // 보정을 위한 위치 벡터 버퍼
						
						// 볼 움직이기
						ball.Move();
						
						/* ~~~~~~~~~~~~~~~~~~~~~충돌 체크하는 부분~~~~~~~~~~~~~~~~~~*/

						// 벽돌과 공 충돌
						double vecBuff2[2] = { 0.0, 0.0 }; // 반사각 계산을 위한 속도 벡터 버퍼
						bool isCollided = false;
						bool isFirstCollision = true;

						for (int i = 0; i < brickList.GetCount(); i++) {

							// 벽돌 한개를 선택하고
							Brick& brick = brickList.GetAt(i);

							// 공의 중심과 벽돌사이의 최단거리중에 벽돌 속에 있는 점을 구한다.
							m_dPointOnRect[0] = Clamp(brick.x1, brick.x2, ball.x);
							m_dPointOnRect[1] = Clamp(brick.y1, brick.y2, ball.y);

							const double distance = GetDistance2(ball.x, ball.y, m_dPointOnRect[0], m_dPointOnRect[1]);

							// 가장 가까운 점과 공 중심의 거리가 공의 반지름보다 작다면 충돌!
							if (distance < pow(Ball::radius, 2.0)) {

								PlaySound(NULL, AfxGetInstanceHandle(), NULL);
								PlaySound((LPCTSTR)IDR_WAVE1, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC | SND_NOSTOP);

								isCollided = true;

								if (isFirstCollision == true) {
									// 처음 충돌한 것에 대해서만 이것을 계산해주자.
									vecBuff2[0] = m_dPointOnRect[0];
									vecBuff2[1] = m_dPointOnRect[1];
									isFirstCollision = FALSE;
								}

								brickList.GetAt(i).life--;
								if (brickList.GetAt(i).life == 0) {
									brickList.RemoveAt(i);
									i--; // 삭제되면 인덱스 한줄 앞당기기
								}

							}
							
						} //여기까지 벽돌과의 충돌 체크

						if (isCollided == true) {

							double wallVec[2];
							// 블록과 공 사이의 충돌면에 대한 법선벡터 (코너에서의 반사각을 계산하기 위함)
							wallVec[0] = ball.x - vecBuff2[0];
							wallVec[1] = ball.y - vecBuff2[1];

							// 반사각을 구한다.
							if (abs(atan2(wallVec[1], wallVec[0]) - atan2(-ball.velocity[1], -ball.velocity[0])) <= (PI/2.0)) {
								// 일반적인 경우는 다음과 같이 계산한다. 벽벡터와 입사각이 예각인 경우이다.
								m_dReflectAngle = 2 * atan2(ball.y - vecBuff2[1], ball.x - vecBuff2[0]) - atan2(-ball.velocity[1], -ball.velocity[0]);
								// 반사각방향으로 공의 스피드를 곱해서 반사될 속도 다시 설정해준다.
								ball.velocity[0] = m_dBallSpeed * cos(m_dReflectAngle);
								ball.velocity[1] = m_dBallSpeed * sin(m_dReflectAngle);
							}
							else {
								// 만약 코너쪽으로 두껍게 들어온다면 다음과 같이 계산한다. 현실에서 불가능한 각도이다.
								//MessageBox(_T("두꺼운각"));
								if (wallVec[0] * ball.velocity[0] < 0) {
									ball.velocity[0] *= -1;
								}
								else {
									ball.velocity[1] *= -1;
								}
							}

							// 공이 벽돌을 파고들어가지 않게 보정 (충돌전 위치로 보정하는 방식)
							ball.SetCenterPoint(vecBuff[0], vecBuff[1]);
						}


						// 프레임과 공 충돌

						// ball의 x1과 x2가 동시에 게임프레임을 탈출하지는 않는다.
						if (ball.x1 <= m_rectGameFrame.TopLeft().x) {
							ball.SetCenterPoint(m_rectGameFrame.TopLeft().x + Ball::radius, ball.y);
							ball.velocity[0] *= -1;
						}
						else if(ball.x2 >= m_rectGameFrame.BottomRight().x){
							ball.SetCenterPoint(m_rectGameFrame.BottomRight().x - Ball::radius, ball.y);
							ball.velocity[0] *= -1;
						}

						// ball의 y1과 y2가 동시에 게임프레임을 탈출하지는 않는다.
						if (ball.y1 <= m_rectGameFrame.TopLeft().y) {
							ball.SetCenterPoint(ball.x, m_rectGameFrame.TopLeft().y + Ball::radius);
							ball.velocity[1] *= -1;
						}
						else if(ball.y2 >= m_rectGameFrame.BottomRight().y){
							ball.SetCenterPoint(ball.x, m_rectGameFrame.BottomRight().y - Ball::radius);
							// 바닥에 닿은 공은 그 자리에 멈춘다.
							ball.isDead = true;
							ball.velocity[0] = 0.0;
							ball.velocity[1] = 0.0;
							m_iDeadBallCount++;
							if (m_iDeadBallCount == m_iBallCnt) {
								m_bIsBallMoving = false;

								// 마지막 공이 떨어진 위치가 새로운 시작점이 된다.
								m_ptStart.SetPoint((int)nearbyint(ball.x), (int)nearbyint(ball.y));
							}
						}
						// 여기까지가 프레임과 공의 충돌

						// 드랍볼과 공의 충돌
						for (int i = 0; i < dropBallList.GetCount(); i++) {
							DropBall& dBall = dropBallList.GetAt(i);
							if (dBall.isValid) {
								if (GetDistance2(ball.x, ball.y, dBall.x, dBall.y) < pow(2 * Ball::radius, 2.0)) {
									m_iDropBallCnt++;
									dBall.Drop();
								}
							}
						}

					}	// 살아있는 한개의 공에 대한 처리 완료

				}	// 살아있는 모든 공에 대한 처리 완료

				// 공이 줄줄이 발사하도록 만든 코드이다. 모든 공이 발사가 되면 이 구문은 실행되지 않는다.
				if (m_iBallCnt > 1 && m_bIsAllBallShooted == false) {
					if (GetDistance2(ballList.GetAt(m_iMovinBallCnt - 1).x, ballList.GetAt(m_iMovinBallCnt - 1).y, ballList.GetAt(m_iMovinBallCnt).x, ballList.GetAt(m_iMovinBallCnt).y) > 500) {

						// 이전에 공이 어느정도 움직인다면 다음 공에 속도를 부여해주고 줄줄이 살려준다.
						ballList.GetAt(m_iMovinBallCnt).velocity[0] = m_dBallSpeed * m_dAimVector[0];
						ballList.GetAt(m_iMovinBallCnt).velocity[1] = m_dBallSpeed * m_dAimVector[1];
						ballList.GetAt(m_iMovinBallCnt).isDead = false;

						// 발산된 공의 갯수를 카운트하는 변수이다.
						m_iMovinBallCnt++;

						if (m_iMovinBallCnt  == m_iBallCnt) {
							m_bIsAllBallShooted = true;
						}
					}
				}
				else if (m_iBallCnt == 1 && m_bIsAllBallShooted == false) { // 공이 한개일때는 예외이다.
					m_bIsAllBallShooted = true;
				}	// 공이 차례대로 발사하도록 만든 코드의 끝이다.

				// 드랍볼과 볼이 모두 멈춘 상태에서 다음 스테이지를 세팅한다.
				if (m_bIsBallMoving == false && m_bIsDBallMoving == false) {
					m_bSettingNextStage = true;
				}
			}
		}

		// m_bSettingNextStage가 TRUE가 되었을때 딱 한번만 실행되는 구문이다. 다음 스테이지를 세팅한다.
		else {
			m_bSettingNextStage = false;

			// 스테이지를 먼저 1증가시킨다.
			m_uiStage++;

			// 스테이지가 1이라면 공을 무조건 추가해준다.
			if (m_uiStage == 1) {
				ballList.Add(Ball());
				m_iBallCnt++;
			}
			else {
				for (int i = 0; i < m_iDropBallCnt; i++) {
					ballList.Add(Ball());
					m_iBallCnt++;
				}
			}

			m_iDropBallCnt = 0;


			// 유효하지 않은 드랍볼들은 삭제한다.
			for (int i = 0; i < dropBallList.GetCount(); i++) {
				DropBall& dBall = dropBallList.GetAt(i);
				if (dBall.isValid == false) {
					dropBallList.RemoveAt(i);
					i--; // 삭제되면 인덱스 한줄 앞당기기
				}
			}

			// 그리고 모든 공을 시작지점으로 모은다.
			for (int i = 0; i < m_iBallCnt; i++) {
				Ball& ball = ballList.GetAt(i);
				ball.SetCenterPoint(m_ptStart.x, m_ptStart.y);
			}

			// 공을 한개만 살려준다.
			ballList.GetAt(0).isDead = false;

			m_iMovinBallCnt = 0;
			m_iDeadBallCount = 0;

			// 기존의 벽돌은 아래로 밀어버린다.
			for (int i = 0; i < brickList.GetCount(); i++) {
				Brick& brick = brickList.GetAt(i);
				brick.MoveOneLine();
			}

			for (int i = 0; i < dropBallList.GetCount(); i++) {
				DropBall& dBall = dropBallList.GetAt(i);
				dBall.MoveOneLine();
			}

			// 새로운 벽돌을 배치한다.
			srand((unsigned int)time(NULL));
			const unsigned char stagePattern = rand() % 31;
			GenerateNewLine(stagePattern);

			// 드랍볼이 스타트 지점에 오면 드랍볼을 획득한것으로 취급
			if (dropBallList.GetAt(0).y > m_rectGameFrame.BottomRight().y - Brick::height) {
				m_iDropBallCnt = 1;
				dropBallList.RemoveAt(0);
			}

			if (brickList.IsEmpty() == false) { // 블럭이 없는 상태에서 CInvalidArgException 방지
				if (brickList.GetAt(0).y2 == m_rectGameFrame.BottomRight().y) {
					// 게임 오버
					m_uiStage--;
					m_iState = END;
					m_bIsRegistered = false;
					m_uiTimeWhenPlayed = (unsigned int)time(NULL);
				}
			}
		}

		break;
	}  // RUNNING

	case END:{

		ballList.RemoveAll();
		brickList.RemoveAll();
		dropBallList.RemoveAll();

		m_ptStart.SetPoint(m_rectGameFrame.CenterPoint().x, m_rectGameFrame.BottomRight().y - (int)Ball::radius);
		m_ptAimLineEnd = m_ptStart; // 처음 조준할때 에임선이 튀는것을 방지

		m_iMovinBallCnt = 0;
		m_iBallCnt = 0;

		break;
	} // END
		
	} // switch
}

void CGameV5View::GenerateNewLine(unsigned char pat)
{
	// 00000 ~ 11110 사이의 비트를 받고
	// 1의 위치에는 벽돌을 채워넣고
	// 0의 위치중 랜덤한 한곳에 드랍볼을 넣는다.

	int emptyCnt = 0; // 빈 공간의 갯수이다.
	int buff = 0;
	int cnt = 0;

	for (int i = 0; i < 5; i++) {
		if (pat & (0x1 << i)) {
			brickList.Add(Brick(m_rectGameFrame.TopLeft().x + Brick::width * i, m_rectGameFrame.TopLeft().y + Brick::height, m_uiStage));
		}
		else {
			emptyCnt++;
		}
	}

	// 빈 공간중에서 buff 번째 공간에 드랍볼을 넣는다.
	buff = rand() % emptyCnt;
	for (int i = 0; i < 5; i++) {
		if ((pat & (0x1 << i)) == 0 ) {
			
			if (cnt == buff) {
				dropBallList.Add(DropBall(m_rectGameFrame.TopLeft().x + Brick::width * i + Brick::width / 2, m_rectGameFrame.TopLeft().y + Brick::height + Brick::height / 2));
				break;
			}
			cnt++;

		}
	}
}


void CGameV5View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	switch (m_iState) {
	case BEGIN: {
		break;
	}
	case RUNNING: {
		if (m_bIsAiming == true) {
			if (point.y > m_ptStart.y) {
				m_ptMouseAim = point;
			}
			else {
				m_ptMouseAim.x = 2 * m_ptStart.x - point.x;
				m_ptMouseAim.y = 2 * m_ptStart.y - point.y;
			}
		}
		break;
	}

	case END: {
		break;
	}
	}

	CView::OnMouseMove(nFlags, point);
}

void CGameV5View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	switch (m_iState) {
	case BEGIN: {
		if (m_rectStartBtn.PtInRect(point)) {
			m_iState = RUNNING;
			m_bSettingNextStage = true;
		}
		else if (m_rectShowRankBtn.PtInRect(point)) {
			ShowRankDlg dlg;
			dlg.DoModal();
		}
		break;
	}
	case RUNNING: {
		if (m_bSettingNextStage == false && m_bIsBallMoving == false && m_bIsDBallMoving == false) {
			if (m_bIsAiming == false && GetDistance(point.x, point.y, m_ptStart.x, m_ptStart.y) < Ball::radius) {
				m_bIsAiming = true;
			}
		}
		break;
	}
	case END: {
		if (m_rectRestartBtn.PtInRect(point) == TRUE ) {
			m_iState = RUNNING;
			m_uiStage = 0;
			m_bSettingNextStage = true;
		}
		else if (m_rectShowRankBtn.PtInRect(point) == TRUE ) {
			m_dlgShowRank.DoModal();
		}

		else if (m_rectRegRankBtn.PtInRect(point) == TRUE ) {
			if (m_bIsRegistered == false) {
				m_dlgRegRank.m_uiScore = m_uiStage;
				m_dlgRegRank.m_uiRecordTime = m_uiTimeWhenPlayed;
				if (m_dlgRegRank.DoModal() == 0) {
					m_bIsRegistered = true;
				}
			}
			else {
				MessageBox(_T("이미 등록하였습니다."));
			}
		}
		break;
	}
	}

	CView::OnLButtonDown(nFlags, point);
}

void CGameV5View::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	switch (m_iState) {
	case BEGIN: {
		break;
	}
	case RUNNING: {
		if (m_bIsAiming == true) {
			m_iMovinBallCnt++;
			m_bIsBallMoving = true;
			m_bIsAiming = false;
			m_bIsAllBallShooted = false;
			ballList.GetAt(0).velocity[0] = m_dBallSpeed * m_dAimVector[0];
			ballList.GetAt(0).velocity[1] = m_dBallSpeed * m_dAimVector[1];
		}
		break;
	}
	case END: {
		break;
	}
			
	}
	CView::OnLButtonUp(nFlags, point);
}