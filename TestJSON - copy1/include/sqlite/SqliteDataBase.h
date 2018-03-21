#pragma once
extern "C"{
#include "..\sqlite\sqlite3.h"
};

class CSqliteTable{
public:
	CSqliteTable(){
		Table_ = NULL;
		Row_ = 0;
		Col_ = 0;
		Index_ = 0;
	};
	~CSqliteTable(){};
public:
	char**& Table(){
		return Table_;
	}

	int& Row(){
		return Row_;
	}

	int& Column(){
		return Col_;
	}
public:
	char* GetValue(int RowNo, int ColNo){
		int Index = RowNo*Col_ + ColNo + Col_;
		return Table_[Index];
	}

	char* GetValue(int ColNo){
		int Index = Index_ + ColNo + Col_;
		return Table_[Index];
	}

	BOOL Step(){
		if (Index_/Col_ >= Row_-1)
			return FALSE;

		Index_ += Col_;
		return TRUE;
	}

	BOOL Seek(int RowNo){
		int Index = RowNo*Col_ + Col_;
		if (Index/Col_ >= Row_)
			return FALSE;

		Index_ = Index;
		return TRUE;	
	}

	int CurrRow(){
		return Index_/Col_;
	}
public:
	char* GetColumnName(int ColNo){
		if (ColNo < Col_)
			return Table_[ColNo];
		return NULL;
	}
public:
	VOID Free(){

		if (Table_ == NULL)
			return ;

		sqlite3_free_table(Table_);
		Table_ = NULL;
		Row_ = 0;
		Col_ = 0;
	}
private:
	int Index_;
private:
	char** Table_;
	int	   Row_;
	int	   Col_;
};


class CSqliteStatement{
public:
	CSqliteStatement(){
		Statement_ = NULL;
	}

	~CSqliteStatement(){

	}
public:
	sqlite3_stmt*& Statement(){
		return Statement_;
	}
public:
	BOOL Bind(int Field, int Value){
		int Rc = sqlite3_bind_int(Statement_, Field, Value);
		return SQLITE_OK == Rc;
	}

	BOOL Bind(int Field, LONG64 Value){
		int Rc = sqlite3_bind_int64(Statement_, Field, Value);
		return SQLITE_OK == Rc;
	}

	BOOL Bind(int Field, double Value)
	{
		int Rc = sqlite3_bind_double(Statement_, Field, Value);
		return SQLITE_OK == Rc;
	}

	BOOL Bind(int Field, LPCWSTR Text)
	{
		int Rc = sqlite3_bind_text16(Statement_, Field, Text, -1, NULL);
		return SQLITE_OK == Rc;
	}

	BOOL Bind(int Field, LPCSTR Text)
	{
		int Rc = sqlite3_bind_text(Statement_, Field, Text, -1, NULL);
		return SQLITE_OK == Rc;
	}

	BOOL Bind(int Field, PVOID Buffer, int Length)
	{
		int Rc = sqlite3_bind_blob(Statement_, Field, Buffer, Length, NULL);
		return SQLITE_OK == Rc;
	}
public:
	int QueryColumnNo(CString& ColName){

		int Cols = sqlite3_column_count(Statement_);
		for(int i = 0 ; i < Cols ; i ++)
		{
			CString Name((LPCWSTR)sqlite3_column_name16(Statement_, i));
			if(ColName.CompareNoCase(Name) == 0)
				return i;
		}
		return -1;
	}

	CString GetText(int Col){
		CString Text((LPCWSTR)sqlite3_column_text16(Statement_, Col));
		return Text;
	}

	CString GetText(CString ColName){
		int Col = QueryColumnNo(ColName);
		if (Col != -1)
		{
			return GetText(Col);
		}
		return _T("");
	}

	int GetInt(int Col){
		return sqlite3_column_int(Statement_, Col);
	}

	int GetInt(CString ColName){
		int Col = QueryColumnNo(ColName);
		if (Col != -1)
		{
			return GetInt(Col);
		}
		return 0;
	}

	LONG64 GetInt64(int Col){
		return sqlite3_column_int64(Statement_, Col);
	}

	LONG64 GetInt64(CString ColName){
		int Col = QueryColumnNo(ColName);
		if (Col != -1)
		{
			return GetInt64(Col);
		}
		return 0;
	}

	double GetDouble(int Col){
		return sqlite3_column_double(Statement_, Col);
	}

	double GetDouble(sqlite3_stmt*& Stmt, CString ColName){
		int Col = QueryColumnNo(ColName);
		if (Col != -1)
		{
			return GetDouble(Col);
		}
		return 0;
	}

	CONST PVOID GetBlob(int Col){
		return (CONST PVOID)sqlite3_column_blob(Statement_, Col);
	}

	CONST PVOID GetBlob(CString ColName){
		int Col = QueryColumnNo(ColName);
		if (Col != -1)
		{
			return GetBlob(Col);
		}
		return NULL;
	}

public:
	BOOL Reset(){
		return SQLITE_OK == sqlite3_reset(Statement_);			
	}

	BOOL Step(){
		return SQLITE_ROW == sqlite3_step(Statement_);
	}

	void ClearBindings(){
		sqlite3_clear_bindings(Statement_);
	}

	void Finalize(){
		sqlite3_clear_bindings(Statement_);
		sqlite3_finalize(Statement_);
		Statement_ = NULL;
	}
private:
	sqlite3_stmt* Statement_;
};


class CSqliteDataBase
{
public:
	CSqliteDataBase(void){
		DataBase_ = NULL;
	}

	~CSqliteDataBase(void){
		Close();
		DataBase_ = NULL;
	}
public:
	BOOL Open(CString& Path){

		CStringA PathA(Path);
		int iRet = sqlite3_open(PathA,&DataBase_);

		if (iRet == SQLITE_OK)
			return TRUE;

		return FALSE;
	}

	VOID Close(){
		if (DataBase_)
			sqlite3_close(DataBase_);
	}

	CONST CString& GetError(){
		return ErrorMsg_;
	}

	BOOL Exec(CString Sql,sqlite3_callback CallBackFunction = NULL,PVOID Context = NULL){
		CStringA SqlA(Sql);
		char* ErrMsg = NULL;
		int Ret = sqlite3_exec(DataBase_,SqlA,CallBackFunction,Context,&ErrMsg);
		if (ErrMsg != NULL)
		{
			CString Error(ErrMsg);
			ErrorMsg_ = Error;
			sqlite3_free(ErrMsg);
		}

		if (Ret == SQLITE_OK)
			return TRUE;

		return FALSE;
	}

	BOOL Select(CString Sql,char**& Result,int& Row, int& Col){
		CStringA SqlA(Sql);
		char* ErrMsg = NULL;

		int Ret = sqlite3_get_table(DataBase_,SqlA,&Result,&Row,&Col,&ErrMsg);

		if (ErrMsg != NULL)
		{
			CString Error(ErrMsg);
			ErrorMsg_ = Error;
			sqlite3_free(ErrMsg);
		}

		if (Ret == SQLITE_OK)
			return TRUE;

		return FALSE;
	}

	BOOL Select(CString Sql, CSqliteTable& SqlResult){
		CStringA SqlA(Sql);
		char* ErrMsg = NULL;
		char**& Result = SqlResult.Table();
		int& Row = SqlResult.Row();
		int& Col = SqlResult.Column();

		int Ret = sqlite3_get_table(DataBase_,SqlA,&Result,&Row,&Col,&ErrMsg);

		if (ErrMsg != NULL)
		{
			CString Error(ErrMsg);
			ErrorMsg_ = Error;
			sqlite3_free(ErrMsg);
		}

		if (Ret == SQLITE_OK)
			return TRUE;

		return FALSE;
	}

public:
	BOOL Prepare(CString Sql,CSqliteStatement& SqlStatement){
		CStringA SqlA(Sql);
		sqlite3_stmt*& Stmt = SqlStatement.Statement();

		int Ret = sqlite3_prepare(DataBase_,SqlA,0,&Stmt,NULL);

		if (Ret == SQLITE_OK)
			return TRUE;

		return FALSE;
	}

public:
	static VOID FreeResult(char**& Result){
		sqlite3_free_table(Result);
		Result = NULL;
	}
private:
	sqlite3*  DataBase_; 
	CString	  ErrorMsg_;
};
