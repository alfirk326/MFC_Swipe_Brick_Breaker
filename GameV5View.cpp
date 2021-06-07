
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
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_iState = BEGIN;
	m_bIsBallMoving = false;
	m_bIsDBallMoving = false;
	m_bIsAiming = false;
	m_bIsAllBallShooted = true;
	m_dAimVector[0] = 0.0 ;
	m_dAimVector[1] = 0.0 ;
	m_dPointOnRect[0] = 0.0;
	m_dPointOnRect[1] = 0.0;
	m_dReflectAngle = 0.0;
	m_ptStart.SetPoint(260, 600);
	m_ptAimLineEnd = m_ptStart;
	m_dBallSpeed = 8.0;
	m_iStage = 0;
	m_iMovinBallCnt = 0;
	m_iBallCnt = 0;
	m_bSettingNextStage = false;
	m_iDeadBallCount = 0;
	m_iDropBallCnt = 0;

	gameFrame.SetRect(10, 60, 510, 610);
	startBtn.SetRect(100, 100, 400, 200);
	guideFrame.SetRect(100, 220, 400, 500);
	restartBtn.SetRect(100, 100, 400, 400);
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

	memDC.CreateCompatibleDC(pDC);
	myBitmap.CreateCompatibleBitmap(pDC, WndRect.Width(), WndRect.Height());
	pOldBitmap = memDC.SelectObject(&myBitmap);
	memDC.PatBlt(0, 0, WndRect.Width(), WndRect.Height(), WHITENESS);

	memDC.Rectangle(gameFrame);

	switch (m_iState) {
	case BEGIN: {

		memDC.Rectangle(startBtn);
		memDC.DrawText(_T("게임시작"), &startBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		
		str.Format(_T("[How to 스와이프 벽돌깨기]\n\n- 스와이프로 공을 날려 벽돌을 파괴하세요.\n- 공이 부딪히면 벽돌의 내구도가 감소합니다.내구도가 0이 되면 벽돌이 파괴됩니다.\n- 녹색 원을 획득하면 공의 수가 늘어납니다.\n- 하단 라인까지 벽돌이 내려오면 게임오버 됩니다."));
		memDC.DrawText(str, &guideFrame, NULL);

		break;
	}
	case RUNNING: {

		if (m_bIsAiming == TRUE) {
			memDC.MoveTo(m_ptStart);
			memDC.LineTo(m_ptAimLineEnd);
		}

		for (int i = 0; i < dropBallList.GetCount(); i++) {
			DropBall& dBall = dropBallList.GetAt(i);
			memDC.Ellipse((int)nearbyint(dBall.x1), (int)nearbyint(dBall.y1), (int)nearbyint(dBall.x2), (int)nearbyint(dBall.y2));
		}

		for (int i = 0; i < ballList.GetCount(); i++) {
			Ball& ball = ballList.GetAt(i);
			memDC.Ellipse((int)nearbyint(ball.x1), (int)nearbyint(ball.y1), (int)nearbyint(ball.x2), (int)nearbyint(ball.y2));
		}


		for (int i = 0; i < brickList.GetCount(); i++) {
			Brick& brick = brickList.GetAt(i);
			int life = brick.life;

			// red컬러를
			//int red  = 255 * m_iStage

			memDC.Rectangle(brick.x1, brick.y1, brick.x2, brick.y2);
			//memDC.FillSolidRect(brick.x1, brick.y1, brick.y2, brick.y2, RGB(200, 0, 0));
			str.Format(_T("%d"), life);

			rect.SetRect(brick.x1, brick.y1, brick.x2, brick.y2);
			memDC.DrawText(str, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		str.Format(_T("x %d"), m_iBallCnt - m_iMovinBallCnt);
		rect.SetRect(m_ptStart.x - 100, m_ptStart.y, m_ptStart.x + 100, m_ptStart.y + 50);
		memDC.DrawText(str, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		break;
	}
	case END: {
		memDC.Rectangle(restartBtn);
		memDC.DrawText(_T("Re?"), &restartBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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
						if (dBall.y2 > gameFrame.BottomRight().y) {
							dBall.SetCenterPoint(dBall.x, gameFrame.BottomRight().y - Ball::radius);
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
						const double vecBuff[2] = { ball.x, ball.y }; // 보정을 위한 "위치" 벡터 버퍼
						
						// 볼 움직이기
						ball.Move();
						
						/* ~~~~~~~~~~~~~~~~~~~~~충돌 체크하는 부분~~~~~~~~~~~~~~~~~~*/

						// 벽돌과 공 충돌
						double vecBuff2[2] = { 0.0, 0.0 }; // 반사각 계산을 위한 "속도" 벡터 버퍼
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

								//PlaySound(NULL, AfxGetInstanceHandle(), NULL);
								//PlaySound((LPCTSTR)IDR_WAVE1, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);

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

						/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~TEST~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
						// m_dReflectAngle = -atan2(ball.y - vecBuff[1], ball.x - vecBuff[0]) + 2 * atan2(-ball.velocity[1], -ball.velocity[0]);
						// -atan2(ball.y - vecBuff[1], ball.x - vecBuff[0]) + atan2(-ball.velocity[1], -ball.velocity[0]) - 3.14/2;

						// CString str;
						// str.Format(_T("%f\n%f\n%f\n%f\n%f\%f\%f"), 
						// 	m_dReflectAngle, ball.x, ball.y, vecBuff[0], vecBuff[1], ball.velocity[0], ball.velocity[1]);
						// MessageBox(str);

						// 이러한 보정법도 괜찮다. (충돌한 방향으로 반지름만큼 공을 이동시키는 방식)
						// ball.SetCenterPoint(vecBuff[0] +  (Ball::radius + 5)*cos(m_dReflectAngle), vecBuff[1] + (Ball::radius + 5) * sin(m_dReflectAngle));
						/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~TEST~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


						// 프레임과 공 충돌

						// ball의 x1과 x2가 동시에 게임프레임을 탈출하지는 않는다.
						if (ball.x1 <= gameFrame.TopLeft().x) {
							ball.SetCenterPoint(gameFrame.TopLeft().x + Ball::radius, ball.y);
							ball.velocity[0] *= -1;
						}
						else if(ball.x2 >= gameFrame.BottomRight().x){
							ball.SetCenterPoint(gameFrame.BottomRight().x - Ball::radius, ball.y);
							ball.velocity[0] *= -1;
						}

						// ball의 y1과 y2가 동시에 게임프레임을 탈출하지는 않는다.
						if (ball.y1 <= gameFrame.TopLeft().y) {
							ball.SetCenterPoint(ball.x, gameFrame.TopLeft().y + Ball::radius);
							ball.velocity[1] *= -1;
						}
						else if(ball.y2 >= gameFrame.BottomRight().y){
							ball.SetCenterPoint(ball.x, gameFrame.BottomRight().y - Ball::radius);
							// 바닥에 닿은 공은 그 자리에 멈춘다.
							ball.isDead = true;
							ball.velocity[0] = 0.0;
							ball.velocity[1] = 0.0;
							m_iDeadBallCount++;
							if (m_iDeadBallCount == m_iBallCnt) {
								m_bIsBallMoving = false;
								m_ptStart.SetPoint((int)nearbyint(ball.x), (int)nearbyint(ball.y));
								//MessageBox(_T("모든공 죽음"));
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

					}// 살아있는 한개의 공에 대한 처리 완료

				} // 살아있는 모든 공에 대한 처리 완료

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
				else if (m_iBallCnt == 1 && m_bIsAllBallShooted == false) {
					m_bIsAllBallShooted = true;
				}
				// 공이 차례대로 발사하도록 만든 코드의 끝이다.


				// 드랍볼과 볼이 모두 멈춘 상태에서 다음 스테이지 세팅을 한다.
				if (m_bIsBallMoving == false && m_bIsDBallMoving == false) {
					m_bSettingNextStage = true;
				}
			}
		}

		// m_bSettingNextStage가 TRUE가 되었을때 딱 한번만 실행되는 구문이다. 다음 스테이지를 세팅한다.
		else {
			m_bSettingNextStage = false;

			// 스테이지를 먼저 1증가시킨다.
			m_iStage++;

			// 스테이지가 1이라면 공을 무조건 추가해준다.
			if (m_iStage == 1) {
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
			if (dropBallList.GetAt(0).y > gameFrame.BottomRight().y - Brick::height) {
				m_iDropBallCnt = 1;
				dropBallList.RemoveAt(0);
			}

			if (brickList.GetAt(0).y2 == gameFrame.BottomRight().y) {
				// 게임 오버
				m_iState = END;
			}

		}
		break;
	}  // RUNNING

	case END:{

		ballList.RemoveAll();
		brickList.RemoveAll();
		dropBallList.RemoveAll();

		m_ptStart.SetPoint(gameFrame.CenterPoint().x, gameFrame.BottomRight().y - (int)Ball::radius);
		m_ptAimLineEnd = m_ptStart; // 처음 조준할때 에임선이 튀는것을 방지

		m_iStage = 0;
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
	int buff = rand();
	int buff2 = 0;

	for (int i = 0; i < 5; i++) {
		if (pat & (0x1 << i)) {
			brickList.Add(Brick(gameFrame.TopLeft().x + Brick::width * i, gameFrame.TopLeft().y + Brick::height, m_iStage));
		}
		else {
			emptyCnt++;
		}
	}
	
	buff %= emptyCnt;
	for (int i = 0; i < 5; i++) {
		if ((pat & (0x1 << i)) == 0 ) {
			
			if (buff2 == buff) {
				dropBallList.Add(DropBall(gameFrame.TopLeft().x + Brick::width * i + Brick::width / 2, gameFrame.TopLeft().y + Brick::height + Brick::height / 2));
				break;
			}
			buff2++;

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
		if (startBtn.PtInRect(point)) {
			m_iState = RUNNING;
			m_bSettingNextStage = true;
		}
		break;
	}
	case RUNNING: {
		if (m_bIsBallMoving == false && m_bIsDBallMoving == false) {
			if (m_bIsAiming == false && GetDistance(point.x, point.y, m_ptStart.x, m_ptStart.y) < Ball::radius) {
				m_bIsAiming = true;
			}
		}
		else {

		}

		break;
	}
	case END: {
		if (restartBtn.PtInRect(point)) {
			m_iState = RUNNING;
			m_bSettingNextStage = true;
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
		if (m_bIsBallMoving == false) {
			if (m_bIsAiming == true) {
				m_iMovinBallCnt++;
				m_bIsBallMoving = true;
				m_bIsAiming = false;
				m_bIsAllBallShooted = false;
				ballList.GetAt(0).velocity[0] = m_dBallSpeed * m_dAimVector[0];
				ballList.GetAt(0).velocity[1] = m_dBallSpeed * m_dAimVector[1];
			}
		}
		break;
	}
	case END: {
		break;
	}

	}
	CView::OnLButtonUp(nFlags, point);
}
