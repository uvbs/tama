// for client
function LogInfo(strInfo) {
    var FileSys = new ActiveXObject("Scripting.FileSystemObject");
    var strLogPath = "\\CCApplicationWizardLog.txt"
    var file = FileSys.OpenTextFile(strLogPath, 8, true);
    file.WriteLine(strInfo);   
    file.Close();        
}

// 디버깅용 로그를 쓰고 싶다면 fileLog.WriteLine()함수를 사용한다. 저장패스는 아래와 같다.
var fileSysLog = new ActiveXObject("Scripting.FileSystemObject");
var fileLog = fileSysLog.CreateTextFile("c:\\xuzhu_work\\xdebugc.txt");

function OnFinish(selProj, selObj) {
    try {
        fileLog.WriteLine("start");
        // Create symbols based on the project name
        var strProjectPath = wizard.FindSymbol('PROJECT_PATH');
        var strProjectName = wizard.FindSymbol('PROJECT_NAME');
        fileLog.WriteLine(strProjectPath);
        fileLog.WriteLine(strProjectName);

        //         var WizardVersion = wizard.FindSymbol('WIZARD_VERSION');
        //         if(WizardVersion >= 8.0)
        //         {
        //         }

        // Create symbols based on the project name
        var strSafeProjectName = CreateSafeName(strProjectName);
        fileLog.WriteLine(strSafeProjectName);
        wizard.AddSymbol("SAFE_PROJECT_NAME", strSafeProjectName);
        wizard.AddSymbol("NICE_SAFE_PROJECT_NAME", strSafeProjectName.substr(0, 1).toUpperCase() + strSafeProjectName.substr(1))
        wizard.AddSymbol("UPPERCASE_SAFE_PROJECT_NAME", strSafeProjectName.toUpperCase());
        fileLog.WriteLine("finish addSymbol");

        // Set current year symbol
        var d = new Date();
        var nYear = 0;
        nYear = d.getFullYear();
        wizard.AddSymbol("CC_CURRENT_YEAR", nYear);
        wizard.AddSymbol("CC_CURRENT_DATE", d.toString());

        // Create project and configurations
        selProj = CreateCustomProject(strProjectName, strProjectPath);
        AddConfigurations(selProj, strProjectName);
        AddFilters(selProj);
        fileLog.WriteLine("finish create project");

        var InfFile = CreateCustomInfFile();
        AddFilesToCustomProj(selProj, strProjectName, strProjectPath, InfFile);
        AddPchSettings(selProj);
        InfFile.Delete();
        fileLog.WriteLine("finish create custom Inf file");

        selProj.Object.Save();
        fileLog.WriteLine("finish save");
        fileLog.Close();
    }
    catch (e) {
        if (e.description.length != 0)
            SetErrorInfo(e);
        return e.number;
    }
}

function CreateCustomProject(strProjectName, strProjectPath) {
    try {
        var strProjTemplatePath = wizard.FindSymbol('PROJECT_TEMPLATE_PATH');
        var strProjTemplate = '';
        var WizardVersion = wizard.FindSymbol('WIZARD_VERSION'); 
        strProjTemplate = strProjTemplatePath + '\\default.vcxproj';

        var Solution = dte.Solution;
        var strSolutionName = "";
        if (wizard.FindSymbol("CLOSE_SOLUTION")) {
            Solution.Close();
            strSolutionName = wizard.FindSymbol("VS_SOLUTION_NAME");
            if (strSolutionName.length) {
                var strSolutionPath = strProjectPath.substr(0, strProjectPath.length - strProjectName.length);
                Solution.Create(strSolutionPath, strSolutionName);
            }
        }

        // Create vcproj.user file
        var FileSys = new ActiveXObject("Scripting.FileSystemObject");
        var strUserTarget = "";
        strUserTarget = strProjectName + ".vcxproj.user";

        var strUserPath = FileSys.BuildPath(strProjectPath, strUserTarget);

        var astrParentPath = new Array();
        astrParentPath[0] = strProjectPath;
        while (astrParentPath.length) {
            var strPath = astrParentPath.pop();
            var strParentPath = FileSys.GetParentFolderName(strPath);

            if (!FileSys.FolderExists(strParentPath)) {
                astrParentPath.push(strPath);
                astrParentPath.push(strParentPath);
                continue;
            } else {
                if (!FileSys.FolderExists(strPath)) {
                    FileSys.CreateFolder(strPath);
                }
                var strWin32ProjectPath = strPath;

                if (!FileSys.FolderExists(strWin32ProjectPath)) {
                    FileSys.CreateFolder(strWin32ProjectPath);
                }
            }
        }

        var file = FileSys.OpenTextFile(strUserPath, 2, true);
        var strUserValue = "";
        strUserValue = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
                + "<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\r\n"
                + "  <PropertyGroup>\r\n"
                + "    <ShowAllFiles>false</ShowAllFiles>\r\n"
                + "    <LocalDebuggerWorkingDirectory Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">..\\..\\..\\win32</LocalDebuggerWorkingDirectory>\r\n"
                + "    <LocalDebuggerWorkingDirectory Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">..\\..\\..\\win32</LocalDebuggerWorkingDirectory>\r\n"
                + "    <DebuggerFlavor Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">WindowsLocalDebugger</DebuggerFlavor>\r\n"
                + "    <DebuggerFlavor Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">WindowsLocalDebugger</DebuggerFlavor>\r\n"
                + "  </PropertyGroup>\r\n"
                + "</Project>";
        file.WriteLine(strUserValue);
        file.Close(); 

        // Create project file
        var strProjectNameWithExt = '';
        strProjectNameWithExt = strProjectName + '.vcxproj';

        var oTarget = wizard.FindSymbol("TARGET");
        var prj;
        if (wizard.FindSymbol("WIZARD_TYPE") == vsWizardAddSubProject)  // vsWizardAddSubProject
        {
            var prjItem = oTarget.AddFromTemplate(strProjTemplate, strProjectNameWithExt);
            prj = prjItem.SubProject;
        }
        else {
            prj = oTarget.AddFromTemplate(strProjTemplate, strProjectPath, strProjectNameWithExt);
        }
        return prj;
    }
    catch (e) {
        throw e;
    }
}

function AddFilters(proj) {
    try {
        // Add the folders to your project
        var strSrcFilter = wizard.FindSymbol('SOURCE_FILTER');
        var group = proj.Object.AddFilter('source');
        group.Filter = strSrcFilter;

        strSrcFilter = wizard.FindSymbol('INCLUDE_FILTER');
        group = proj.Object.AddFilter('include');
        group.Filter = strSrcFilter;

        strSrcFilter = wizard.FindSymbol('RESOURCE_FILTER');
        group = proj.Object.AddFilter('resource');
        group.Filter = strSrcFilter;
    }
    catch (e) {
        throw e;
    }
}

// Configurations data
var nNumConfigs = 2;

var astrConfigName = new Array();
astrConfigName[0] = "Debug";
astrConfigName[1] = "Release";

function AddConfigurations(proj, strProjectName) {
    try {
        var nCntr;
        for (nCntr = 0; nCntr < nNumConfigs; nCntr++) {
            // Check if it's Debug configuration
            var bDebug = false;
            if (astrConfigName[nCntr].search("Debug") != -1)
                bDebug = true;

            // General settings
            var config = proj.Object.Configurations(astrConfigName[nCntr]);

            config.CharacterSet = charSetUnicode;
            config.useOfMfc = useMfcDynamic;

            var WizardVersion = wizard.FindSymbol('WIZARD_VERSION'); 
            config.OutputDirectory = '$(SolutionDir)$(Configuration)\\'
            config.IntermediateDirectory = '$(Configuration)\\';

            // Compiler settings
            var CLTool = config.Tools('VCCLCompilerTool');

            // Additional Inlcude Directories
            var strAddIncludeDir = '.;..\\..\\..\\..\\..\\XE\\Common\\_XLibrary';
            strAddIncludeDir += ';..\\..\\..\\game_src';
            strAddIncludeDir += ';..\\..\\..\\Resource';
            strAddIncludeDir += ';..\\..\\..\\..\\XFramework';
            strAddIncludeDir += ';..\\..\\..\\..\\..\\XE\\Third_Party_lib';
            strAddIncludeDir += ';..\\..\\..\\..\\..\\XE\\Third_Party_lib\\bugslayer\\Include';
            strAddIncludeDir += ';..\\..\\..\\..\\..\\XE\\Third_Party_lib\\Lua\\5.1\\include';
            strAddIncludeDir += ';..\\..\\..\\..\\..\\XE\\Third_Party_lib\\pthread_win32\\include';
            strAddIncludeDir += ';..\\..\\..\\..\\..\\XE\\XFramework';
            
            CLTool.AdditionalIncludeDirectories = strAddIncludeDir;

            CLTool.UsePrecompiledHeader = pchUseUsingSpecific;  // pchUseUsingSpecific;
            CLTool.WarningLevel = warningLevel_3;
            if (bDebug) {
                CLTool.RuntimeLibrary = rtMultiThreadedDebugDLL;
                CLTool.MinimalRebuild = true;
                CLTool.DebugInformationFormat = debugEditAndContinue;
                CLTool.BasicRuntimeChecks = runtimeBasicCheckAll;
                CLTool.Optimization = optimizeDisabled;
            }
            else {
                CLTool.RuntimeLibrary = rtMultiThreadedDLL;
                CLTool.ExceptionHandling = false;
                CLTool.DebugInformationFormat = debugDisabled;
            }

            var strDefines = GetPlatformDefine(config);
            strDefines += "WIN32;_WINDOWS;";
            if (bDebug)
                strDefines += "_DEBUG;DEBUG";
            else
                strDefines += "NDEBUG";
            CLTool.PreprocessorDefinitions = strDefines;

            // Disable special warning
//            CLTool.DisableSpecificWarnings = "4267;4251;4244";

            // Linker settings
            var LinkTool = config.Tools('VCLinkerTool');
            LinkTool.SubSystem = subSystemWindows;
            LinkTool.TargetMachine = machineX86;
            if (bDebug) {
                LinkTool.LinkIncremental = linkIncrementalYes;
                LinkTool.GenerateDebugInformation = true;
            }
            else {
                LinkTool.LinkIncremental = linkIncrementalNo;
            }

            // Additional Library Directories
            var strAddDepends = 'xe_clientd.lib windowscodecs.lib winmm.lib d3d9.lib d3dx9d.lib d3dx10d.lib d3dxof.lib dxerr.lib dsound.lib';
            if (bDebug) {
                LinkTool.AdditionalLibraryDirectories = '..\\..\\..\\..\\..\\XE\\bin\\Debug';
                //strAddDepends += ' xe_client.lib';
            }
            else {
                LinkTool.AdditionalLibraryDirectories = '..\\..\\..\\..\\..\\XE\\bin\\Release';
                //strAddDepends += ' xe_client.lib';
            }
            LinkTool.AdditionalDependencies = strAddDepends;

            // Resource settings
            //var RCTool = config.Tools("VCResourceCompilerTool");
            //RCTool.Culture = rcEnglishUS;
            //RCTool.AdditionalIncludeDirectories = "$(IntDir)";
            //if (bDebug)
            //    RCTool.PreprocessorDefinitions = "_DEBUG";
            //else
            //    RCTool.PreprocessorDefinitions = "NDEBUG";

            // MIDL settings
            //var MidlTool = config.Tools("VCMidlTool");
            //MidlTool.MkTypLibCompatible = false;
            //if (IsPlatformWin32(config))
            //    MidlTool.TargetEnvironment = midlTargetWin32;
            //if (bDebug)
            //    MidlTool.PreprocessorDefinitions = "_DEBUG";
            //else
            //    MidlTool.PreprocessorDefinitions = "NDEBUG";
            //MidlTool.HeaderFileName = strProjectName + ".h";
            //MidlTool.InterfaceIdentifierFileName = strProjectName + "_i.c";
            //MidlTool.ProxyFileName = strProjectName + "_p.c";
            //MidlTool.GenerateStublessProxies = true;
            //MidlTool.TypeLibraryName = "$(IntDir)/" + strProjectName + ".tlb";
            //MidlTool.DLLDataFileName = "";

            // Post-build settings
//            var PostBuildTool = config.Tools("VCPostBuildEventTool");
//            PostBuildTool.Description = "Performing copy resource from Resource to OutDir...";
//            PostBuildTool.CommandLine = "xcopy /E /Q /Y \"$(ProjectDir)Resource\\*.*\" \"$(OutDir)\"";
        }
    }
    catch (e) {
        throw e;
    }
}

function AddPchSettings(proj) {
    try {
        var files = proj.Object.Files;
        var fStdafx = files("StdAfx.cpp");
         
        var nCntr;
        for(nCntr = 0; nCntr < nNumConfigs; nCntr++)
        {
            var config = fStdafx.FileConfigurations(astrConfigName[nCntr]);
            config.Tool.UsePrecompiledHeader = pchCreateUsingSpecific;
        }
    }
    catch (e) {
        throw e;
    }
}

function DelFile(fso, strWizTempFile) {
    try {
        if (fso.FileExists(strWizTempFile)) {
            var tmpFile = fso.GetFile(strWizTempFile);
            tmpFile.Delete();
        }
    }
    catch (e) {
        throw e;
    }
}

function CreateCustomInfFile() {
    try {
        var fso, TemplatesFolder, TemplateFiles, strTemplate;
        fso = new ActiveXObject('Scripting.FileSystemObject');

        var TemporaryFolder = 2;
        var tfolder = fso.GetSpecialFolder(TemporaryFolder);

        var strWizTempFile = tfolder.Path + "\\" + fso.GetTempName();

        var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');
        var strInfFile = strTemplatePath + '\\Templates.inf';
        wizard.RenderTemplate(strInfFile, strWizTempFile);

        var WizTempFile = fso.GetFile(strWizTempFile);

        return WizTempFile;
    }
    catch (e) {
        throw e;
    }
}

function GetTargetName(strName, strProjectName) {
    try {
        var strTarget = strName;
        // rootXXX.cpp와같은 파일들의 이름을 strProjectName으로 교체한다.
        var nIndex = strName.indexOf("root");
        if (nIndex >= 0) {
            strTarget = strName.substring(0, nIndex) + strProjectName + strName.substring(nIndex + 4, strName.length);
        }
        // Templates.inf파일에서 game_src/XXXX와 같은 목록은 앞에 패스를 추가해서 ../../../game_src/xxxx로 만들어준다.
        // game_src/, Resource/, win32/ work/등의 폴더는 XE/prj/App에 있기때문이다.. 폴더명을 App으로 한것은 AppTemplate/App/pc_prj의 이름과 일치시키기 위함이다.
        fileLog.WriteLine("++GetTargetName::");
        // 다음 단어가 들어간 목록은 앞에 ../를 덧붙여서 리턴한다.
        var nIndex1 = strTarget.indexOf("game_src/");
        var nIndex2 = strTarget.indexOf("Resource/");   // Resource폴더도 앞에 ../를 붙임.
        var nIndex3 = strTarget.indexOf("win32/");   // win32폴더도 앞에 ../를 붙임.
        var nIndex4 = strTarget.indexOf("Work/");   
        if (nIndex1 >= 0 || nIndex2 >= 0 || nIndex3 >= 0 || nIndex4 >= 0) {
            fileLog.WriteLine("+ ../" + strTarget);
            strTarget = "../" + strTarget;
        }
        fileLog.WriteLine( strTarget );
        return strTarget;
    }
    catch (e) {
        throw e;
    }
}
function AddFilesToCustomProj(proj, strProjectName, strProjectPath, InfFile) {
    try {
        var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');
        var strTpl = '';
        var strName = '';
        var strTextStream = InfFile.OpenAsTextStream(1, -2);
        // TEmplates.inf의 목록을 돌면서 처리한다.
        while (!strTextStream.AtEndOfStream) {
            strTpl = strTextStream.ReadLine();
            // 빈 공백라인은 건너뜀.
            if (strTpl != '') {
                fileLog.WriteLine(strTpl);
                strName = strTpl;
                var refFile = false;
                // &붙은 파일은 프로젝트에 포함시키지 않고 카피만 하도록 한다.
                var nIndex = strName.indexOf("&");
                if( nIndex >= 0 ) {     // &를 떼낸다.
                    refFile = true;     // 파일카피만 하고 프로젝트엔 포함시키지 않음.
                    strName = strName.substring(nIndex+1, strName.length)
                }
                // TEmplates.inf의 패스앞에 ../를 하나 더 붙여서 프로젝트루트폴더를 기준으로 생성되도록 한다.
                var strTarget = GetTargetName(strName, strProjectName);
                // XEAppWiz.XXX/Templates/1033 + / + "../../../App/game_src/xxxxx
                var strTemplate = strTemplatePath + '\\' + strName;
                // SamplePrj/App/pc_prj/Game/Game + / + "../../../../App/game_src/xxxxx
                var strFile = strProjectPath + '\\' + strTarget;
                // 파일을 해당 폴더에 카피하며 디폴트로는 프로젝트에도 포함시킴.
                var bCopyOnly = false;  // true가 되면 프로젝트에 링크되지 않고 파일카피만 이뤄진다. "true" will only copy the file from strTemplate to strTarget without rendering/adding to the project
                if (refFile == true) {    // &붙은 파일은
                    fileLog.WriteLine("+bCopyOnly for & = true");
                    bCopyOnly = true;  // 파일카피만 하고 프로젝트에 넣지 않는다.
                } else {
                    // 리소스폴더 파일은 &안붙어도 프로젝트에 포함시키지 않고 파일카피만 한다.
                    // 단 defineGame.h같은 파일들은 &를 붙이지 않는다.
                    fileLog.WriteLine("+is App/Resource");
                    var nIdxRes = strTarget.indexOf("App/Resource");
                    if (nIdxRes >= 0) {
                        bCopyConly = true;
                        fileLog.WriteLine("+bCopyOnly for App/Resource = true");
                    }
                    nIdxRes = strTarget.indexOf("win32/");
                    if (nIdxRes >= 0) {
                        bCopyConly = true;
                        fileLog.WriteLine("+bCopyOnly for win32/ = true");
                    }
                    // .indexOf함수는 대소문자를 가리는듯 하다 확실하지 않음.
                    nIdxRes = strTarget.indexOf("Work/");
                    if (nIdxRes >= 0) {
                        bCopyConly = true;
                        fileLog.WriteLine("+bCopyOnly for work/ = true");
                    }
                    fileLog.WriteLine("+c/cpp/h/rc check");
                    // 파일명부분은 떼냄
                    var strExt = strName.substr(strName.lastIndexOf("."));
                    // h/cpp/c/rc파일은 
                    if (strExt != ".h" && strExt != ".cpp" && strExt != ".c" && strExt != ".rc") {
                        bCopyOnly = true;
//                    if (strExt == ".h" || strExt == ".cpp" || strExt == ".c" || strExt == ".rc") {
//                        bCopyOnly = false;
                        fileLog.WriteLine("+bCopyOnly = false; not h/cpp/c/rc ");
                    }
                }
                fileLog.WriteLine("+RenderTemplate log");
                fileLog.WriteLine(strTemplatePath);
                fileLog.WriteLine(strTemplate);
                fileLog.WriteLine(strFile);
                fileLog.WriteLine("bCopyOnly=" + bCopyOnly);
                // 프로젝트에 파일을 보함시킨다. 
                // 폴더에 카피하거나 프로젝트에 넣을 파일들은 일단 이걸 먼저 해줘야 한다. 이걸 하지 않으면 파일이 아예 카피가 안된다.
                var bDontProcess = false;
                var bOverwrite = true;
                if (bCopyOnly)
                    bDontProcess = true;
                fileLog.WriteLine("bDontProcess=" + bDontProcess);
                // "렌더"란 [!output PROJECT_NAME]같은거를 프로젝트명으로 자동변환시켜주고/파일을 카피하는 걸 말한다.(구글에서 RenderTemplate함수검색 참조)
                // [!output PROJECT_NAME]같은거 bDontProcess=true로 해서 처리안하면 프로젝트이름으로 변환안됨.
                // 따라서 c/cpp/h/rc파일 이외의 파일이 bDontProcess=false가 되면 에러가 남
                // 모든 카피되는 파일은 RenderTemplate을 거쳐야 함.단지 bDontProcess가 다를 뿐임.
                fileLog.WriteLine("+RenderTemplate start");
                wizard.RenderTemplate(strTemplate, strFile, bDontProcess, bOverwrite);  // true is overwrite
                fileLog.WriteLine("+RenderTemplate end");
                // don't add these files to the project
                if (strTarget == strProjectName + ".h" ||
                   strTarget == strProjectName + "ps.mk" ||
                   strTarget == strProjectName + "ps.def")
                    continue;
                if (bCopyOnly)  // &붙은 파일과 bCopyOnly인 파일들은 프로젝트에 넣지 않는다.
                    continue;
                // 파일을 프로젝트에 포함시킨다.
                fileLog.WriteLine("+AddFile");
                proj.Object.AddFile(strFile);
            }
        }
        strTextStream.Close();
    }
    catch (e) {
        throw e;
    }
}
