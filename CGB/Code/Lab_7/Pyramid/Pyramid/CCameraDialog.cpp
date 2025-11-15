#include "pch.h"
#include "Pyramid.h"
#include "afxdialogex.h"
#include "CCameraDialog.h"

IMPLEMENT_DYNAMIC(CCameraDialog, CDialogEx)

CCameraDialog::CCameraDialog(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CAMERA_DIALOG, pParent)
    , m_r(100.0)    
    , m_fi(315.0)   
    , m_q(45.0)     
{
}

CCameraDialog::~CCameraDialog()
{
}

void CCameraDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    DDX_Text(pDX, IDC_EDIT_R, m_r);
    DDX_Text(pDX, IDC_EDIT_FI, m_fi);
    DDX_Text(pDX, IDC_EDIT_Q, m_q);

    if (pDX->m_bSaveAndValidate)
    {
        if (m_r < 10.0 || m_r > 200.0)
        {
            AfxMessageBox(_T("Радиус r должен быть в диапазоне от 10 до 200"), MB_ICONERROR);
            pDX->Fail();
        }
        if (m_fi < -180.0 || m_fi > 180.0)
        {
            AfxMessageBox(_T("Азимут φ (fi) должен быть в диапазоне от -180 до 180 градусов"), MB_ICONERROR);
            pDX->Fail();
        }
        if (m_q < 0.0 || m_q > 180.0)
        {
            AfxMessageBox(_T("Угол возвышения θ (q) должен быть в диапазоне от 0 до 180 градусов"), MB_ICONERROR);
            pDX->Fail();
        }
    }
}

BEGIN_MESSAGE_MAP(CCameraDialog, CDialogEx)
    ON_BN_CLICKED(IDOK, &CCameraDialog::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CCameraDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    UpdateData(FALSE);

    GetDlgItem(IDC_EDIT_R)->SetWindowText(CString(std::to_wstring(m_r).c_str()));
    GetDlgItem(IDC_EDIT_FI)->SetWindowText(CString(std::to_wstring(m_fi).c_str()));
    GetDlgItem(IDC_EDIT_Q)->SetWindowText(CString(std::to_wstring(m_q).c_str()));

    return TRUE;
}

void CCameraDialog::OnBnClickedOk()
{
    if (!UpdateData(TRUE))
    {
        return;
    }

    // Все проверки пройдены
    CDialogEx::OnOK();
}