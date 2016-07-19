// for client
function LogInfo(strInfo) {
    var FileSys = new ActiveXObject("Scripting.FileSystemObject");
    var strLogPath = "\\CCApplicationWizardLog.txt"
    var file = FileSys.OpenTextFile(strLogPath, 8, true);
    file.WriteLine(strInfo);   
    file.Close();        
}

// ������ �α׸� ���� �ʹٸ� fileLog.WriteLine()�Լ��� ����Ѵ�. �����н��� �Ʒ��� ����.
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
        // rootXXX.cpp�Ͱ��� ���ϵ��� �̸��� strProjectName���� ��ü�Ѵ�.
        var nIndex = strName.indexOf("root");
        if (nIndex >= 0) {
            strTarget = strName.substring(0, nIndex) + strProjectName + strName.substring(nIndex + 4, strName.length);
        }
        // Templates.inf���Ͽ��� game_src/XXXX�� ���� ����� �տ� �н��� �߰��ؼ� ../../../game_src/xxxx�� ������ش�.
        // game_src/, Resource/, win32/ work/���� ������ XE/prj/App�� �ֱ⶧���̴�.. �������� App���� �Ѱ��� AppTemplate/App/pc_prj�� �̸��� ��ġ��Ű�� �����̴�.
        fileLog.WriteLine("++GetTargetName::");
        // ���� �ܾ �� ����� �տ� ../�� ���ٿ��� �����Ѵ�.
        var nIndex1 = strTarget.indexOf("game_src/");
        var nIndex2 = strTarget.indexOf("Resource/");   // Resource������ �տ� ../�� ����.
        var nIndex3 = strTarget.indexOf("win32/");   // win32������ �տ� ../�� ����.
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
        // TEmplates.inf�� ����� ���鼭 ó���Ѵ�.
        while (!strTextStream.AtEndOfStream) {
            strTpl = strTextStream.ReadLine();
            // �� ��������� �ǳʶ�.
            if (strTpl != '') {
                fileLog.WriteLine(strTpl);
                strName = strTpl;
                var refFile = false;
                // &���� ������ ������Ʈ�� ���Խ�Ű�� �ʰ� ī�Ǹ� �ϵ��� �Ѵ�.
                var nIndex = strName.indexOf("&");
                if( nIndex >= 0 ) {     // &�� ������.
                    refFile = true;     // ����ī�Ǹ� �ϰ� ������Ʈ�� ���Խ�Ű�� ����.
                    strName = strName.substring(nIndex+1, strName.length)
                }
                // TEmplates.inf�� �н��տ� ../�� �ϳ� �� �ٿ��� ������Ʈ��Ʈ������ �������� �����ǵ��� �Ѵ�.
                var strTarget = GetTargetName(strName, strProjectName);
                // XEAppWiz.XXX/Templates/1033 + / + "../../../App/game_src/xxxxx
                var strTemplate = strTemplatePath + '\\' + strName;
                // SamplePrj/App/pc_prj/Game/Game + / + "../../../../App/game_src/xxxxx
                var strFile = strProjectPath + '\\' + strTarget;
                // ������ �ش� ������ ī���ϸ� ����Ʈ�δ� ������Ʈ���� ���Խ�Ŵ.
                var bCopyOnly = false;  // true�� �Ǹ� ������Ʈ�� ��ũ���� �ʰ� ����ī�Ǹ� �̷�����. "true" will only copy the file from strTemplate to strTarget without rendering/adding to the project
                if (refFile == true) {    // &���� ������
                    fileLog.WriteLine("+bCopyOnly for & = true");
                    bCopyOnly = true;  // ����ī�Ǹ� �ϰ� ������Ʈ�� ���� �ʴ´�.
                } else {
                    // ���ҽ����� ������ &�Ⱥپ ������Ʈ�� ���Խ�Ű�� �ʰ� ����ī�Ǹ� �Ѵ�.
                    // �� defineGame.h���� ���ϵ��� &�� ������ �ʴ´�.
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
                    // .indexOf�Լ��� ��ҹ��ڸ� �����µ� �ϴ� Ȯ������ ����.
                    nIdxRes = strTarget.indexOf("Work/");
                    if (nIdxRes >= 0) {
                        bCopyConly = true;
                        fileLog.WriteLine("+bCopyOnly for work/ = true");
                    }
                    fileLog.WriteLine("+c/cpp/h/rc check");
                    // ���ϸ�κ��� ����
                    var strExt = strName.substr(strName.lastIndexOf("."));
                    // h/cpp/c/rc������ 
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
                // ������Ʈ�� ������ ���Խ�Ų��. 
                // ������ ī���ϰų� ������Ʈ�� ���� ���ϵ��� �ϴ� �̰� ���� ����� �Ѵ�. �̰� ���� ������ ������ �ƿ� ī�ǰ� �ȵȴ�.
                var bDontProcess = false;
                var bOverwrite = true;
                if (bCopyOnly)
                    bDontProcess = true;
                fileLog.WriteLine("bDontProcess=" + bDontProcess);
                // "����"�� [!output PROJECT_NAME]�����Ÿ� ������Ʈ������ �ڵ���ȯ�����ְ�/������ ī���ϴ� �� ���Ѵ�.(���ۿ��� RenderTemplate�Լ��˻� ����)
                // [!output PROJECT_NAME]������ bDontProcess=true�� �ؼ� ó�����ϸ� ������Ʈ�̸����� ��ȯ�ȵ�.
                // ���� c/cpp/h/rc���� �̿��� ������ bDontProcess=false�� �Ǹ� ������ ��
                // ��� ī�ǵǴ� ������ RenderTemplate�� ���ľ� ��.���� bDontProcess�� �ٸ� ����.
                fileLog.WriteLine("+RenderTemplate start");
                wizard.RenderTemplate(strTemplate, strFile, bDontProcess, bOverwrite);  // true is overwrite
                fileLog.WriteLine("+RenderTemplate end");
                // don't add these files to the project
                if (strTarget == strProjectName + ".h" ||
                   strTarget == strProjectName + "ps.mk" ||
                   strTarget == strProjectName + "ps.def")
                    continue;
                if (bCopyOnly)  // &���� ���ϰ� bCopyOnly�� ���ϵ��� ������Ʈ�� ���� �ʴ´�.
                    continue;
                // ������ ������Ʈ�� ���Խ�Ų��.
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
