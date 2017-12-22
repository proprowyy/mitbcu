/*
 * Copyright (c) 2012���MIT
 * All right reserved.
 *
 * ��������������� touch_screen_intercomm.h
 * ���������������
 * ���    ������  ������������������������������������
 *
 * ��������������� V1.0.0
 * ���    ������ wilson
 * ���������������2013-01-11
 *
 * ���������������
 * ���������  ���
 * ���������������
 */
#ifndef TSINTERCOMM_h
#define TSINTERCOMM_h

#include "ts_head_file.h"

extern char d2p_request_buffer[32][20];
extern unsigned char currend_d2P_position;
extern unsigned char is_intercomming_with_pcu;
extern unsigned char connecting_d2p_response_sursor;
extern char current_recept_pcu_no_buffer_language[30];
extern char current_recept_pcu_no_buffer[30];
extern int current_is_being_d2d;

/** SetReceptPCUNo function description
 *  ���������������������
 *
 *  @param   ������PCU������������
 *  @return  null
 */
void SetReceptPCUNo(unsigned char param_pcu_position,unsigned char vn);
void SetReceptPCUNo_ISCS(unsigned char param_pcu_position,unsigned char vn);
/** InitPCURequestTable function description
 *  ���������PCU���������
 *港
 *  @param   null
 *  @return  null
 *
//void InitPCURequestTable();

/** UpdataPCURequestTable function description
 *  ������PCU���������
 *
 *  @param   ������������������PCU������������
 *  @param   PCU���������
 *  @param   PCU������������
 *  @return  null
 */
void UpdataPCURequestTable(unsigned char param_add_or_remove,unsigned char param_pcu_no,unsigned char param_request_sequence);

/** ShowD2PRequest function description
 *  ������PCU������������
 *
 *  @param   null
 *  @return  null
 */
void ShowD2PRequest();
void ShowD2DRequest();
//void AdjustD2PRequestSequence(int param_pcu_no);

/** SetD2PCmd function description
 *  ������D2P������������
 *
 *  @param   ������������������D2P
 *  @param   ������������������0
 *  @param   PCU���������
 *  @param   BCU���������
 *  @return  null
 */
void SetD2PCmd(int enter_or_exit,int param_response_type,int param_pcu_no,int param_bcu_no);

/** ChangeIntercommLabelState function description
 *  ������������������������������
 *
 *  @param   ������������������������������������
 *  @return  null
 */
void ChangeIntercommLabelState(int param_current_intercomm_state);

/** ClearAllPCURequest function description
 *  ������������PCU������
 *
 *  @param   null
 *  @return  null
 */
//void ClearAllPCURequest();

/** ChangeD2DBreakUpD2PBtnState function description
 *  ������D2D������D2P������
 *
 *  @param   null
 *  @return  null
 */
void ChangeD2DBreakUpD2PBtnState();

/** RecoverD2DBreakUpD2PBtnState function description
 *  ������D2D������D2P������
 *
 *  @param   null
 *  @return  null
 */
void RecoverD2DBreakUpD2PBtnState();

/** ShowNoPCURequest function description
 *   ������������PCU ������������
 *
 *  @param   null
 *  @return  null
 */
void ShowNoPCURequest();
void disable_recv_refuse();
#endif
