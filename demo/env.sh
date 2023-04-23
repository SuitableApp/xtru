#!/bin/bash
# ===================================================================
# Copyright (c) 2023 SuitableApp
# ===================================================================
#	=======================================================
#	XTRU のコマンド引数として使用する環境変数
#	=======================================================
	export XTRU_USERID=SYSTEM/MANAGER
#	export XTRU_USERID=@userid.crd
#	=======================================================
#	XTRU の内部で使用する環境変数
#	=======================================================
	export XTRU_SRC_USER=SCOTT
	export XTRU_DST_INFO=SCOTT/TIGER
#	export XTRU_DST_INFO=@dst_info.crd
	export XTRU_ALTER_DEST=
	export XTRU_LISTEXCPT=
	export XTRU_LISTFIXED=
	export XTRU_LISTTABLE=
	export XTRU_BULK_SIZE=1000
	export XTRU_MIN_BULK_SIZE=100
	export XTRU_MAXLONGSIZE=1000
	export XTRU_MINIMUM_FREE_SIZE=1024M
	export XTRU_INPUT=%XTRU_USER_DIR%input
	export XTRU_OUTPUT=%XTRU_USER_DIR%output
	export XTRU_AUXILIARY_DEST=
	export XTRU_DFILE_ALT_DIRS=
	export XTRU_STDOUT=0x0000
#	-------------------------------------------------------
# O: %OUTPUT%, T: Table Name, P: Partition Name, C: T+'_'+P
# A: %OUTPUT%,  %AUXILIARY_DEST% or %DFILE_ALT_DIRS%
# I: %DST_INFO%, X: Filename Extention
# D=yyyyMMdd: Date, W=HHmmss: Time, E=ENV_VAL_NAME
#	-------------------------------------------------------
	export XTRU_STREAM_LOCATOR=
#	export XTRU_STREAM_LOCATOR=ipc_pipe://zip "{O}\{C}_{D=yyyyMMdd}_{W=HHmmss}_{E=SRC_USER}_{X}" -v -
#	export XTRU_STREAM_LOCATOR=ipc_pipe://7z a -t7z -mx1 -si{C}.{X} "{O}/{C}"
#	export XTRU_STREAM_LOCATOR=named_pipe://\\.\pipe\{C}.{X}
	export XTRU_QUERYFIX=qryfix
	export XTRU_QUERYVAR=qryvar
	export XTRU_QUERYFILENAME=*
	export XTRU_FILEBIND=bind.dat
	export XTRU_FILEEXCPT=excpt.dat
	export XTRU_FILEFIXED=fixed.dat
	export XTRU_FILEFKRB=fkrb.sql
	export XTRU_FILELOG=
	export XTRU_FILETRACE=
	export XTRU_FILEMETA=meta.sql
	export XTRU_FILEPLSN=plsn.txt
	export XTRU_FILEPLSP=plsp.txt
	export XTRU_FILESIZE=2000M
	export XTRU_FILETABLE=table.dat
	export XTRU_FILETBLN=tbln.txt
	export XTRU_FILETBLP=tblp.txt
	export XTRU_LOADMETHOD=TRUNCATE
	export XTRU_SKIPSCR=
	export XTRU_ROWS=true
	export XTRU_NUM_ROWS=1
	export XTRU_METADIV=
	export XTRU_EXEC_LOAD=sqlldr
	export XTRU_EXEC_PLUS=sqlplus
	export XTRU_DATE_MASK=YYYYMMDDHH24MISS
	export XTRU_TIMESTAMP_MASK=YYYYMMDDHH24MISSXFF
	export XTRU_TIMESTAMP_TZ_MASK=YYYYMMDDHH24MISSXFFTZHTZM
	export XTRU_EMBED_COLUMN_NAME=
	export XTRU_TERMINATOR=,
	export XTRU_ENCLOSER=~~
	export XTRU_ENCLOSER2=
	export XTRU_RECLENGTH=true
	export XTRU_ENDTERM=true
	export XTRU_DISCRETE=true
	export XTRU_USUALPATH=true
	export XTRU_RECOVERABLE=true
	export XTRU_SORTING=
	export XTRU_FEEDBACK=10
	export XTRU_CMNTLVL=1
	export XTRU_LONGTRANSIT=0
	export XTRU_DIRALIAS=ALIAS1
	export XTRU_COMPRESS=true
	export XTRU_CONSISTENT=true
	export XTRU_EXTNAMEBLOB=blo
	export XTRU_EXTNAMECLOB=clo
	export XTRU_EXTNAMESQL=sql
	export XTRU_WINTARGET=true
	export XTRU_DATAEXT=
	export XTRU_DEPENDENT=true
	export XTRU_MNGSCROUT=0xec00
	export XTRU_SORT_AREA_SIZE=
	export XTRU_OPTIMIZER_MODE=
	export XTRU_NMENCLS=true
	export XTRU_FINDSTRCMD=
	export XTRU_SQLFMT=0x8000
	export XTRU_RERUNPOINT=
	export XTRU_GETEXTTBL=
	export XTRU_BIND_SIZE=20M
	export XTRU_FBQSCN=
	export XTRU_FBQTIME=
	export XTRU_FPNUMFMT=
	export XTRU_PRINTCOLID=true
	export XTRU_NUMDAYS_AUDIT=
	export XTRU_PRE_REP_EXEC_PLS=PreRepPls.sql
	export XTRU_POST_REP_EXEC_PLS=PostRepPls.sql
	export XTRU_TAF_DELAY_TIME=60
	export XTRU_MERGE_LOBS_INTO_SDF=
#	=======================================================
#	アドバンスト・チューニング
#	-------------------------------------------------------
# PARTITIONING: ビットがマークされた時出力が分割されます。
# 1-bit: パーティション表と集合化クエリ
# 2-bit: ROWID 範囲によるヒープ表分割
# from 3 to 15-bit: 未使用
# true は 1, 2 をマークした値、つまり 3 と同じ意味
# 指定無 は 1, 2 をクリアした値、つまり 0 と同じ意味
#	-------------------------------------------------------
# FILE_MAPPING: 0:Not use, 1:WRITE_THROUGH, 2:WRITE_BACK
#	-------------------------------------------------------
# PRIORITY_CLASS:  IDLE, BELOW_NORMAL, NORMAL, 
#                  ABOVE_NORMAL, HIGH, REALTIME
#	-------------------------------------------------------
# THREAD_PRIORITY: IDLE, LOWEST, BELOW_NORMAL, NORMAL,
#                  ABOVE_NORMAL ,HIGHEST, TIME_CRITICAL
#	=======================================================
	export XTRU_PARALLELISM=4
	export XTRU_IO_OVERLAP_SCALE=1
	export XTRU_OVERLAP_BUFFER_LENGTH=1M
	export XTRU_DB_FILE_MULTIBLOCK_READ_COUNT=128
	export XTRU_PARTITIONING=3
	export XTRU_OPTIMIZER_HINTS=
	export XTRU_ROWID_SPLIT_MIN_SIZE=0M
	export XTRU_ROWID_SPLIT_NUM_PARTS=4
	export XTRU_FILE_MAPPING=0
	export XTRU_PRIORITY_CLASS=BELOW_NORMAL
	export XTRU_THREAD_PRIORITY=BELOW_NORMAL
	export XTRU_NUM_ASYNCLOBS=%PARALLELISM%
#	=======================================================
#	AL32UTF8:873, UTF8:871, NLS_LANGに依存:0
#	NOBYTEORDERMARK は UTF-8 系 CHARSET 選択時のみ有効
#	=======================================================
	export XTRU_CHARSETID=0
	export XTRU_NOBYTEORDERMARK=
#	=======================================================
#	メインフレーム向けデータ出力
#	-------------------------------------------------------
#	NUMBER2BCD: 7: COMP-3, 91: DISPLAY SIGN LEADING SEPARATE
#	ROWSEPARATOR: 0x00: 行セパレータ(CR+LF)を出力しない
#	CHARSETFORM: 2: NCHAR/NVARCHAR を UTR-16 で出力
#	REVERSEBYTEORDER: true: バイト順序を倒置 (Big Endian化)
#	=======================================================
	export XTRU_NUMBER2BCD=
	export XTRU_ROWSEPARATOR=
	export XTRU_CHARSETFORM=
	export XTRU_REVERSEBYTEORDER=
#	=======================================================
#	診断出力
#	=======================================================
	export XTRU_CLIENT_ID=XTRU_CLIENT
	export XTRU_SERVEROUTPUT=
	export XTRU_WAIT_SYNC_INTERVAL=
	export XTRU_WAIT_SYNC_MAXRETRY=
	export XTRU_MINIDUMP_TYPE=0x0
	export XTRU_PROTECT_FROM=
#	-------------------------------------------------------
#	exec DBMS_MONITOR.CLIENT_ID_TRACE_ENABLE ('XTRU_CLIENT');
#	exec DBMS_MONITOR.SERV_MOD_ACT_TRACE_ENABLE('<service_name>','xtru.exe');
#	trcsess output=xtru.trc clientid=XTRU_CLIENT module=xtru.exe <SID>_ora_*.trc
#	tkprof xtru.trc xtru.txt sys=no
#	-------------------------------------------------------
	export XTRU_SHOWSTMT=true
	export XTRU_SHOWCLTDTM=
	export XTRU_SHOWSTAT=
	export XTRU_SHOWVERS=
#	=======================================================
#	使用する実行形式ファイル
#	=======================================================
	export XTRU_EXECUTABLE=%XTRU_EXEC_DIR%xtru.exe
	export XTRU_EXEC_MPX=%XTRU_EXEC_DIR%mpx.exe

