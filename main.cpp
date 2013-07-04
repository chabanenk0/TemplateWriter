#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <Shlobj.h>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
HINSTANCE hInst;

//#DEFINE IDB_BTGETTEMPLDIR 101
/*  Make the class name into a global variable  */
char szClassName[ ] = "WindowsApp";

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);
    hInst=hThisInstance;

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "Template Writer",       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           475,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nFunsterStil);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

HWND h1,h2,h3,h4,h5,h6,h7,h8,h9,h10;

	static OPENFILENAME ofn;
	static char szFile[MAX_PATH];
LPBROWSEINFO lpbi;


void get_template_list(char *szDisplayName)
{// функция открывает каталог и считывает список всех его подкаталогов
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind;
   char sPath[2048];

    //Specify a file mask. *.* = We want everything!
    sprintf(sPath, "%s\\*.*", szDisplayName);

   hFind = FindFirstFileEx(sPath, FindExInfoStandard, &FindFileData,
             FindExSearchNameMatch, NULL, 0);

   SendMessage(h3,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);
   if (hFind == INVALID_HANDLE_VALUE) 
   {
      //printf ("FindFirstFileEx failed (%d)\n", GetLastError());
      return;
   } 
   else 
   {
        do{
            if(strcmp(FindFileData.cFileName, ".") !=  0
                && strcmp(FindFileData.cFileName, "..") != 0
                &&(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))

       SendMessage(h3,LB_ADDSTRING,(WPARAM)NULL,(LPARAM)FindFileData.cFileName);
       }
      while (FindNextFile(hFind,&FindFileData));
      FindClose(hFind);
   } 
}

void copy_text(FILE *fp_dest,FILE *fp_source)
{char s[1000];
int n;
while (!feof(fp_source))
 {
     fgets(s,999,fp_source);
     //MessageBox((HWND)0, s , "Text string read", MB_OK);
     n=strlen(s);
     s[n-2]=0;
     fprintf(fp_dest,"<p>%s</p>",s);
 }
}

void process_template(char *templatefullname,char *inputfullname,char *mask)
{// Функция, реализирующая идею шаблона. 
char s3[300],ch1;
int n,n_mask,n_cmp,n_total,n_begmask;
strcpy(s3,templatefullname);
n=strlen(s3);
n_mask=strlen(mask);
s3[n-1]='_';

FILE*fp1=fopen(s3,"w");
FILE*fp2=fopen(templatefullname,"r");
FILE*fp3=0;
n_cmp=0;
n_total=0;
while (!feof(fp2))
  {
      ch1=fgetc(fp2);
      n_total++;
      if (ch1==mask[n_cmp])
       {//MessageBox((HWND)0, "Letters equal" , "Letters", MB_OK);
        if (n_cmp==0)n_begmask=n_total;
           n_cmp++;
           if (n_cmp==n_mask)
             {// нашли маску, переписываем второй файл
               //MessageBox((HWND)0, "Mask found" , "Mask found", MB_OK);
               fp3=fopen(inputfullname,"r");
               fseek(fp1,-n_mask+1,SEEK_CUR);
               copy_text(fp1,fp3);
               fclose(fp3);
               n_cmp=0;
             }                
       }
       else
       n_cmp=0;
       fputc(ch1,fp1);
  }
fclose(fp2);
fclose(fp1);
DeleteFile(templatefullname);
MoveFile(s3,templatefullname);
}


LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{BOOL success;
BROWSEINFO              bi;
TCHAR                   szDisplayName[MAX_PATH]; 
LPITEMIDLIST            pidl; 
char initdir[200];
char filedir[200];
char templatedir[200];
char outputdir[200];
char templatefile[200];
char s[300];
char s1[300];
char templatefullname[300];
char inputfullname[300];
char labeltext[100];
char tempdir[300];
char tempfile[300];
		    HKEY hkRegKey;
            _TCHAR szBuf[300];
            DWORD dwBufLen = 300;
            int numruns;
            int l;
            FILE *f;

            LPMALLOC  pMalloc = NULL;
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            h1=CreateWindow(TEXT("button"),"Выбрать папку шаблонов",WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
            30,30,200,30,hwnd,(HMENU)101,hInst,NULL);
            h2=CreateWindow(TEXT("edit"),"",WS_BORDER|WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON|ES_LEFT,
            80,70,150,30,hwnd,(HMENU)102,hInst,NULL);
            CreateWindow(TEXT("static"),"Папка шаблонов",WS_CHILD|WS_VISIBLE,
            10,70,70,30,hwnd,(HMENU)103,hInst,NULL);
            h3=CreateWindow(TEXT("listbox"),"",WS_CHILD|WS_VISIBLE| WS_VSCROLL |LBS_DISABLENOSCROLL,
            30,120,150,200,hwnd,(HMENU)104,hInst,NULL);

            h4=CreateWindow(TEXT("button"),"Выбрать файлы текстов",WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
            50+230,30,200,30,hwnd,(HMENU)105,hInst,NULL);
            h5=CreateWindow(TEXT("edit"),"",WS_BORDER|WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON|ES_LEFT,
            50+280,70,150,30,hwnd,(HMENU)106,hInst,NULL);
            CreateWindow(TEXT("static"),"файлы",WS_CHILD|WS_VISIBLE,
            50+210,70,70,30,hwnd,(HMENU)107,hInst,NULL);
            h6=CreateWindow(TEXT("listbox"),"",WS_CHILD|WS_VISIBLE| WS_VSCROLL |LBS_DISABLENOSCROLL,
            50+230,120,150,200,hwnd,(HMENU)108,hInst,NULL);

            h7=CreateWindow(TEXT("button"),"Выбрать выходную папку",WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
            330,370,200,30,hwnd,(HMENU)109,hInst,NULL);
            h8=CreateWindow(TEXT("edit"),"",WS_BORDER|WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON|ES_LEFT,
            100,370,240,30,hwnd,(HMENU)110,hInst,NULL);
            CreateWindow(TEXT("static"),"Выходная папка",WS_CHILD|WS_VISIBLE,
            20,370,60,30,hwnd,(HMENU)111,hInst,NULL);

            h9=CreateWindow(TEXT("button"),"Выполнить",WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
            330,400,200,30,hwnd,(HMENU)111,hInst,NULL);

            h10=CreateWindow(TEXT("edit"),"<%%%TEXT%%%>",WS_BORDER|WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON|ES_LEFT,
            100,320,240,30,hwnd,(HMENU)110,hInst,NULL);
            CreateWindow(TEXT("static"),"Метка",WS_CHILD|WS_VISIBLE,
            20,320,60,30,hwnd,(HMENU)111,hInst,NULL);


       		ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = hwnd;
		    ofn.lpstrFile = szFile;
		    ofn.nMaxFile = sizeof(szFile);
		    GetEnvironmentVariable("temp",tempdir,300);
		    sprintf(tempfile,"%s\\f001.txt",tempdir);
		     		      //MessageBox(hwnd, "Создаем файл" , tempfile, MB_OK);
		    f=fopen(tempfile,"r");
		    if (!f)
		     {f=fopen(tempfile,"w");
 		      //MessageBox(hwnd, "Создаем файл" , tempfile, MB_OK);
		      fprintf(f,"10");
		      fclose(f);
               }
             else
             {fscanf(f,"%d",&numruns);
              numruns--;
              fclose(f);
              f=fopen(tempfile,"w");
		      fprintf(f,"%d",numruns);
		      fclose(f);
		      if (numruns<1)
		      {MessageBox(hwnd, "Тестовый период окончен. Пора бы оплатить обещанные 10$ (icq:345243743)" , "!!!", MB_OK);
		      PostQuitMessage (0);
		      return 0;
                            }
             }  

            break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 101 :	
			strcpy(szFile, "");
            ZeroMemory(&bi, sizeof(bi));      
            bi.hwndOwner = NULL; 
            bi.pszDisplayName = szDisplayName; 
            bi.lpszTitle = TEXT("Select folder"); 
            bi.ulFlags = BIF_RETURNONLYFSDIRS;  
            GetCurrentDirectory(200,initdir);
            //bi.pidlRoot=initdir;
            pidl = SHBrowseForFolder(&bi); 
            if  (pidl)
            {
                	SHGetPathFromIDList(pidl, szDisplayName); 	
                    //AfxMessageBox(szDisplayName);
            }

			//success = GetOpenFileName(&ofn);
			//if (success)
			//	MessageBox(hwnd, ofn.lpstrFile, "Открывается файл:", MB_OK);
			//else
			//MessageBox(hwnd, "GetOpenFileName",
			//    szDisplayName , MB_ICONWARNING);
			SendMessage(h2,WM_SETTEXT,(WPARAM)NULL,(LPARAM)szDisplayName);
			get_template_list(szDisplayName);
			break;
        
		case 105 :	
        	ofn.Flags= OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
            GetCurrentDirectory(200,initdir);
        	ofn.lpstrInitialDir=initdir;
        	ofn.lpstrFilter="Text Files\0*.txt\0All Files\0*.*\0\0";
        	success = GetOpenFileName(&ofn);
             if (success) {
                //SendMessage(h6,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);
             for (char *i=ofn.lpstrFile+ofn.nFileOffset-1;!((i[0]=='\0')&&(i[1]=='\0'));i=strchr(i,0))
                 {i++;
                        SendMessage(h6,LB_ADDSTRING,(WPARAM)NULL,(LPARAM)i);
                   //MessageBox(hwnd, i , "Открывается файл:", MB_OK);
                 }
                 ofn.lpstrFile[ofn.nFileOffset]=0;
                 SendMessage(h5,WM_SETTEXT,(WPARAM)NULL,(LPARAM)ofn.lpstrFile);
              // string[] selectedFiles =  ofn.file.Split(new char[] { '\0' });
                
                //return selectedFiles;
            } else {
                return NULL;}
			    break;
		case 109 :	
			strcpy(szFile, "");
            ZeroMemory(&bi, sizeof(bi));      
            bi.hwndOwner = NULL; 
            bi.pszDisplayName = szDisplayName; 
            bi.lpszTitle = TEXT("Select folder"); 
            bi.ulFlags = BIF_RETURNONLYFSDIRS;  
            GetCurrentDirectory(200,initdir);
            //bi.pidlRoot=initdir;
            pidl = SHBrowseForFolder(&bi); 
            if  (pidl)
            {
                	SHGetPathFromIDList(pidl, szDisplayName); 	
                    //AfxMessageBox(szDisplayName);
            }

			//success = GetOpenFileName(&ofn);
			//if (success)
			//	MessageBox(hwnd, ofn.lpstrFile, "Открывается файл:", MB_OK);
			//else
			//MessageBox(hwnd, "GetOpenFileName",
			//    szDisplayName , MB_ICONWARNING);
			SendMessage(h8,WM_SETTEXT,(WPARAM)NULL,(LPARAM)szDisplayName);

			break;
   		case 111 :	
//char filedir[200];
//char templatedir[200];
//char outputdir[200];
//char templatefile[200];

             int n=SendMessage(h6,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
             int n_templ=SendMessage(h3,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
             int i_templ;
             SendMessage(h5,WM_GETTEXT,(WPARAM)200,(LPARAM)initdir);
 //			MessageBox(hwnd, "ititdir", initdir , MB_ICONWARNING);
             SendMessage(h2,WM_GETTEXT,(WPARAM)200,(LPARAM)templatedir);
//              			MessageBox(hwnd, "templatedir", templatedir , MB_ICONWARNING);

             SendMessage(h8,WM_GETTEXT,(WPARAM)200,(LPARAM)outputdir);
             SendMessage(h10,WM_GETTEXT,(WPARAM)200,(LPARAM)labeltext);
             int n_len=strlen(labeltext);
             labeltext[n_len]=0;
//              			MessageBox(hwnd, "outputdir", outputdir , MB_ICONWARNING);
              			MessageBox(hwnd,  labeltext , "labeltext",MB_ICONWARNING);
             for(int i=0;i<n;i++)
             {SendMessage(h6,LB_GETTEXT,(WPARAM)i,(LPARAM)szFile);
              //strcat(initdir,szFile);//initdir содержит путь к файлу...
              i_templ=rand()%n_templ;
              SendMessage(h3,LB_GETTEXT,(WPARAM)i_templ,(LPARAM)templatefile);
              SetCurrentDirectory(outputdir);
//              			MessageBox(hwnd, "GetOpenFileName",
//			    szDisplayName , MB_ICONWARNING);

              strcpy(s,szFile);
              char* k=strstr(s,".txt");
              *k=0;
// 			MessageBox(hwnd,s, "s"  , MB_ICONWARNING);
              CreateDirectory(s,NULL);// нужно бы без .тхт
              sprintf(s1,"xcopy /E /Y %s\\%s\\*.* %s\\",templatedir,templatefile,s);
    //           			MessageBox(hwnd,s1, "s1"  , MB_ICONWARNING);
              //WinExec(s1);//copy templatedir/templatefile outputdir/szFile
              //ShellExecute(hwnd,"open","cmd.exe", s1,NULL,SW_SHOWNORMAL);//"/C hg > test.txt"), that's just a clumsy imitation of system()
              system(s1);
              sprintf(templatefullname,"%s\\%s\\index.html",outputdir,s);
     //			MessageBox(hwnd,templatefullname, "templatefullname"  , MB_ICONWARNING);
              sprintf(inputfullname,"%s\\%s",initdir,szFile);
       	//		MessageBox(hwnd,inputfullname, "inputfullname"  , MB_ICONWARNING);
              process_template(templatefullname,inputfullname,labeltext);
             }
             MessageBox(hwnd,"Работа сделана.", "Работа сделана."  , MB_ICONWARNING);
             break;

          }

            break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
