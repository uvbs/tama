using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data;
using System.IO;
using System.Net;
using System.Reflection;
using System.Net.Json;
using System.Net.Security;
using System.Security.Cryptography.X509Certificates;
using System.Timers;
using System.Data.SqlClient;
using System.Data.OleDb;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using INI;

namespace PushApplication
{    
    class Program
    {
        private const string DEVICE_ID = "";
        private string strUri = "https://android.googleapis.com/gcm/send";            //google url

//        private string DBIp = "192.168.0.192";
         private string DBIp = "127.0.0.1";
//        private string DBIp = "211.125.93.154";
        private string DBName = "Caribe";
        private string DBId = "Caribe";
        private string DBPass = "Mobile099!";

        
        //private string API_KEY = "AIzaSyCSr8aUCvk-Qij0NBRrBKGLfizpGSzBtWw";
//        private string API_KEY = "AIzaSyDGo3mEIx6ykmWSr0AFZzwkvYvZBywW8QA"; //caribe
        private string API_KEY = "AIzaSyCAnAgzJXDtYJSjBTX1uTR2vQ6NScQmTFA"; //caribe
        

        private const int APPLY_COUNT = 1000;
        private int PUSH_RATE = 5;
        private Int64 LastDeviceindex = 0;

        private struct PUSHMSGInfo
        {
            public UInt32 IdAccount;
            public string deviceid;
            public string type1;
            public string type2;
            public string pushmessage;
        };
        private struct DeviceInfo
        {
            public UInt32 IdAccount;
            public Int32 index;
            public string deviceid;            
        };

        private Dictionary<Int64, DeviceInfo> DicDeviceID;
        //private List<string> DeviceIDList;
        private List<PUSHMSGInfo> PushMSGList;

        static void Main(string[] args)
        {
            Program App = new Program();

            App.Run();
        }

        private void Run()
        {
            int n = 0;
            while (n == 0)
            {
                LoadDeviceIDDB();
                LoadBookedPushMSG();
                DeleteBookedPushMSG();
                ProcessBookedPUSHMSG();
                Thread.Sleep(2000);
            }            
        }      

        private static object Lock = new object(); 
        private List<Task> tasks = new List<Task>();
        private CancellationTokenSource cancellationTokenSource;
        private void push_Elapsed(object sender, ElapsedEventArgs e)
        {
            //DB 에 처리 해야 하는Push가 있는지 확인 해 보고.
            LoadBookedPushMSG();
            ProcessBookedPUSHMSG();
        }
        private void LoadIniFilePath()
        {
            
            FileInfo exefileinfo = new FileInfo(System.IO.Path.GetDirectoryName(Environment.GetCommandLineArgs()[0]));
            string path = exefileinfo.Directory.FullName.ToString();  //프로그램 실행되고 있는데 path 가져오기
            string fileName = @"\Config.ini";  //파일명

            iniUtil ini = new iniUtil(path + fileName);   // 만들어 놓았던 iniUtil 객체 생성(생성자 인자로 파일경로 정보 넘겨줌)

            string DBIP = ini.GetIniValue("PUSHDB", "IP");
            string DBNAME = ini.GetIniValue("PUSHDB", "DBNAME");
            string DBID = ini.GetIniValue("PUSHDB", "ID");
            string DBPASS = ini.GetIniValue("PUSHDB", "PASS");

            string apikey = ini.GetIniValue("GCMN", "API_KEY");

            int pushterm = Convert.ToInt32(ini.GetIniValue("GCMN", "PUSH_RATE"));

            if (DBIP.Length > 0) DBIp = DBIP;
            if (DBNAME.Length > 0) DBName = DBIP;
            if (DBID.Length > 0) DBId = DBIP;
            if (DBPASS.Length > 0) DBPass = DBIP;
            if (apikey.Length > 0) API_KEY = apikey;
            if (pushterm > 0) PUSH_RATE = pushterm;
        }
        private bool LoadDeviceIDDB()
        {
            string UserIndex = string.Empty;
            try
            {
                using (SqlConnection con = new SqlConnection())
                {
                    con.ConnectionString = string.Format("Server={0};DataBase={1};Uid={2};Pwd={3}", this.DBIp, this.DBName, this.DBId, this.DBPass);
                    string sqlquery = "SELECT [index], idaccount, deviceid FROM dbo.tbl_deviceid WHERE dbo.tbl_deviceid.[index] > " + LastDeviceindex.ToString() + " AND  dbo.tbl_deviceid.appstore = 'google'";

                    SqlCommand cmd = new SqlCommand(sqlquery, con);
                    con.Open();

                    if (this.DicDeviceID == null)
                        this.DicDeviceID = new Dictionary<Int64, DeviceInfo>();
                  
                    SqlDataReader rd = cmd.ExecuteReader();

                    bool bChange = false;
                    while (rd.Read())
                    {
                        string strindex = rd["index"].ToString();
                        Int64 Index64 = Convert.ToInt64(strindex);

                        string stridaccount = rd["idaccount"].ToString();
                        string deviceid = rd["deviceid"].ToString();
                        UInt32 idaccount = Convert.ToUInt32( stridaccount );
                                                
                        DeviceInfo Data = new DeviceInfo();
                        Data.IdAccount = idaccount;
                        Data.deviceid = deviceid;
                        Data.index = Convert.ToInt32(strindex);                        
        
                        DicDeviceID.Add(Index64, Data);
                        LastDeviceindex = Index64;
                        bChange = true;
                    }
                    rd.Close();
                    con.Close();

                    

                    if( bChange )
                    {
                        DateTime currTime = DateTime.Now;
                        Console.WriteLine("[" + currTime.ToString("HH:mm:ss") + "] LastDeviceindex= " + LastDeviceindex.ToString());
                    }                   
                
                    return true;
                }
            }
            catch (Exception ex)
            {
                DateTime currTime = DateTime.Now;
                Console.WriteLine("[" + currTime.ToString("HH:mm:ss") + "] LoadDeviceIDDB()" + ex.Message);
            }
            return false;
        }
        private string GetDeviceList(UInt32 idaccount , ref int index)
        {
            try
            {
                foreach (KeyValuePair<Int64, DeviceInfo> each in this.DicDeviceID)
                {
                    if (idaccount == each.Value.IdAccount && index != each.Value.index) 
                    {
                        index = each.Value.index;
                        return each.Value.deviceid;
                    }
                }

                return null;
            }
            catch (Exception ex)
            {
                DateTime currTime = DateTime.Now;
                Console.WriteLine("[" + currTime.ToString("HH:mm:ss") + "] GetDeviceList() " + ex.Message);
                return null;
            }
        }
        private UInt32 GetDeviceCount(UInt32 idaccount)
        {
            UInt32 n32Count = 0;
            try
            {
                foreach (KeyValuePair<Int64, DeviceInfo> each in this.DicDeviceID)
                {
                    if (idaccount == each.Value.IdAccount) n32Count++;
                }

                return n32Count;
            }
            catch (Exception ex)
            {
                DateTime currTime = DateTime.Now;
                Console.WriteLine("[" + currTime.ToString("HH:mm:ss") + "] GetDeviceList() " + ex.Message);
                return 0;
            }
        }        
        private void DeleteBookedPushMSG()
        {           
            try
            {
                using (SqlConnection con = new SqlConnection())
                {
                    con.ConnectionString = string.Format("Server={0};DataBase={1};Uid={2};Pwd={3}", this.DBIp, this.DBName, this.DBId, this.DBPass);
                    string sqlquery = "DELETE dbo.tbl_pushmsg WHERE dbo.tbl_pushmsg.sendtime < GETDATE()";

                    SqlCommand cmd = new SqlCommand(sqlquery, con);
                    con.Open();
                    SqlDataReader rd = cmd.ExecuteReader();                    
                    rd.Close();
                    con.Close();
                }

//                 PUSHMSGInfo Data1 = new PUSHMSGInfo();                
//                 UInt32 u32IdAccount = 1000;
//                 if (u32IdAccount > 0)
//                 {
//                     Data1.IdAccount = u32IdAccount;
//                     Data1.type1 = "1";// rd["type"].ToString();
//                     Data1.type2 = "1";// rd["type"].ToString();
//                     Data1.deviceid = "APA91bGeBE2XYOkTZeL-gHrAC0ST6-9M73DLxSUkHe3CNYCzQEf4UVnVSLceQwv9MS56cdJKp_Rp8K5RrZ96BcQfM3mbTfketN7lOQUPnh65fdbdSMwn1IU19Oi59B-9JonipNti9_GNeVMTqC96qMxJ0DbwKTX7jg";//GetDeviceList(IdAccount);
//                     Data1.pushmessage = "Hello EveryOne";
//                     PushMSGList.Add(Data1);
//                 }

            }
            catch (Exception ex)
            {
                DateTime currTime = DateTime.Now;
                Console.WriteLine("[" + currTime.ToString("HH:mm:ss") + "] DeleteBookedPushMSG() " + ex.Message);
            }
        }
        private void LoadBookedPushMSG()
        {
            string UserIndex = string.Empty;
            try
            {
                if (this.PushMSGList == null)
                    this.PushMSGList = new List<PUSHMSGInfo>();

                PushMSGList.Clear();

                using (SqlConnection con = new SqlConnection())
                {
                    con.ConnectionString = string.Format("Server={0};DataBase={1};Uid={2};Pwd={3}", this.DBIp, this.DBName, this.DBId, this.DBPass);
                    string sqlquery = "SELECT idaccount, type1, type2, pushmessage, sendtime FROM dbo.tbl_pushmsg WHERE dbo.tbl_pushmsg.sendtime < GETDATE() ORDER BY  dbo.tbl_pushmsg.sendtime DESC";

                    SqlCommand cmd = new SqlCommand(sqlquery, con);
                    con.Open();

                    SqlDataReader rd = cmd.ExecuteReader();
                    while (rd.Read())
                    {
                        PUSHMSGInfo Data = new PUSHMSGInfo();
                        string stridaccount = rd["idaccount"].ToString();
                        UInt32 IdAccount = Convert.ToUInt32(stridaccount);

                        if (IdAccount > 0)
                        {
                            foreach (KeyValuePair<Int64, DeviceInfo> each in this.DicDeviceID)
                            {
                                if (IdAccount == each.Value.IdAccount)
                                {
                                    Data.IdAccount = IdAccount;
                                    Data.type1 = rd["type1"].ToString();
                                    Data.type2 = rd["type2"].ToString();
                                    UInt32 type2 = Convert.ToUInt32(stridaccount);
                                    Data.deviceid = each.Value.deviceid;

                                    Data.pushmessage = rd["pushmessage"].ToString();
                                    PushMSGList.Add(Data);
                                }
                            }
                        }
                            
                    }
                    rd.Close();
                    con.Close();
                }

//                 PUSHMSGInfo Data1 = new PUSHMSGInfo();                
//                 UInt32 u32IdAccount = 1000;
//                 if (u32IdAccount > 0)
//                 {
//                    Data1.IdAccount = u32IdAccount;
//                    Data1.type = "1";// rd["type"].ToString();
//                    Data1.deviceid = "APA91bGeBE2XYOkTZeL-gHrAC0ST6-9M73DLxSUkHe3CNYCzQEf4UVnVSLceQwv9MS56cdJKp_Rp8K5RrZ96BcQfM3mbTfketN7lOQUPnh65fdbdSMwn1IU19Oi59B-9JonipNti9_GNeVMTqC96qMxJ0DbwKTX7jg";//GetDeviceList(IdAccount);
//                    Data1.pushmessage = "Hello EveryOne";
//                    PushMSGList.Add(Data1);
//                 }
//                 PUSHMSGInfo Data1 = new PUSHMSGInfo();
//                 UInt32 u32IdAccount = 1000;
//                 if (u32IdAccount > 0)
//                 {
//                     Data1.IdAccount = u32IdAccount;
//                     Data1.type1 = "1";// rd["type"].ToString();
//                     Data1.type2 = "1";// rd["type"].ToString();
//                     Data1.deviceid = "APA91bHELKNHPFrZEslHDF3vlpdRQwYzxz-tc5FEjPxUPvzoLG6jRQfz-3kgNovBQNKmuKNGS8x0FX8pUxmIYWDFzCm0NB3eghaRG0h-lDh0I2JStwjaOq39rT1ypLLYzQO34tPLP8kl";//GetDeviceList(IdAccount);
//                     Data1.pushmessage = "제목/Hello EveryOne";
//                     PushMSGList.Add(Data1);
//                }

            }
            catch (Exception ex)
            {
                DateTime currTime = DateTime.Now;
                Console.WriteLine("[" + currTime.ToString("HH:mm:ss") + "] LoadBookedPushMSG()" + ex.Message);
            }
        }       
        private void ProcessBookedPUSHMSG()
        {
            try
            {
                if (PushMSGList.Count > 0)
                {
                    DateTime currTime = DateTime.Now;
                    Console.WriteLine("[" + currTime.ToString("HH:mm:ss") + "] (ProcessBookedPUSHMSG()) " + PushMSGList.Count.ToString());

                    tasks = new List<Task>();
                    int index = 0;
                    foreach (PUSHMSGInfo each in PushMSGList)
                    {
                        tasks.Add(Task.Factory.StartNew(() =>
                        {
                            lock (Lock)
                            {
                                index++;
                            }
                            if (each.deviceid.Equals("all") || each.deviceid.Equals("All") || each.deviceid.Equals("ALL") || each.deviceid.Equals("1")) //전체 푸쉬.
                            {
                                //                        if (bLoad == false)
                                //                            bLoad = LoadDeviceIDDB();
                                PluralPostData(each.pushmessage);
                            }
                            else //전체 아니니까 바로 푸쉬 해주도록 한다.
                            {
                                SinglePostData(each.deviceid, each.pushmessage);
                            }
                            //SendMessage("[Make" + currTime.ToString("HH:mm:ss") + " [" + index + "] " + p.pushmessage);

                        }));                        
                    }
                    Task.WaitAll(tasks.ToArray());
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("[ERROR] ProcessBookedPUSHMSG  " + ex.Message);
             //   Console.WriteLine(ex.ToString() + "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
            }
            
        }
        private void SinglePostData(string deviceid, string pushmessage)
        {
            string PostData = string.Empty;

            JsonObjectCollection totalCollection = new JsonObjectCollection();
            JsonObjectCollection Collection = new JsonObjectCollection("data");
            //JsonObjectCollection dryrunCollection = new JsonObjectCollection("dry_run");
            JsonArrayCollection arrayCollection = new JsonArrayCollection("registration_ids");

            //Collection.Add(new JsonStringValue(null, type1));
            //Collection.Add(new JsonStringValue(null, type2));
            Collection.Add(new JsonStringValue("msg", pushmessage));

            arrayCollection.Add(new JsonStringValue(null, deviceid));           
//            arrayCollection.Add(new JsonStringValue(null, "APA91bHELKNHPFrZEslHDF3vlpdRQwYzxz-tc5FEjPxUPvzoLG6jRQfz-3kgNovBQNKmuKNGS8x0FX8pUxmIYWDFzCm0NB3eghaRG0h-lDh0I2JStwjaOq39rT1ypLLYzQO34tPLP8kl"));           
            
            totalCollection.Add(arrayCollection);
            totalCollection.Add(Collection);

            SendGCMNotification(API_KEY, totalCollection.ToString());
        }
        private void PluralPostData(string message)
        {
            string postData = string.Empty;

            int count = this.DicDeviceID.Count;

            if (count > APPLY_COUNT)
            {
                int nCount = 0;

                JsonObjectCollection totalCollection = new JsonObjectCollection();
                JsonObjectCollection Collection = new JsonObjectCollection("data");
                JsonArrayCollection arrayCollection = new JsonArrayCollection("registration_ids");

                //Collection.Add(new JsonStringValue("type", type));
                Collection.Add(new JsonStringValue("msg", message));

                foreach (KeyValuePair<Int64, DeviceInfo> each in this.DicDeviceID)
                {                    
                    if (nCount > APPLY_COUNT)
                    {
                        totalCollection.Add(arrayCollection);
                        totalCollection.Add(Collection);

                        SendGCMNotification(API_KEY, totalCollection.ToString());

                        arrayCollection.Clear();
                        nCount = 0;
                    }
                    arrayCollection.Add(new JsonStringValue(null, each.Value.deviceid));
                    nCount++;
                }
                totalCollection.Add(arrayCollection);
                totalCollection.Add(Collection);

                SendGCMNotification(API_KEY, totalCollection.ToString());
            }
            else
            {
                JsonObjectCollection Collection = new JsonObjectCollection("data");
                JsonObjectCollection totalCollection = new JsonObjectCollection();
                JsonArrayCollection arrayCollection = new JsonArrayCollection("registration_ids");

                foreach (KeyValuePair<Int64, DeviceInfo> each in this.DicDeviceID)
                {
                    arrayCollection.Add(new JsonStringValue(null, each.Value.deviceid));
                }

                Collection.Add(new JsonStringValue(null, message));
                totalCollection.Add(arrayCollection);
                totalCollection.Add(Collection);

                SendGCMNotification(API_KEY, totalCollection.ToString());
            }

            SendGCMNotification(API_KEY, postData);
        }
        public static bool ValidateServerCertificate(object sender, X509Certificate certificate, X509Chain chain, SslPolicyErrors sslPolicyErrors)
        {
            return true;
        }
        private string SendGCMNotification(string apiKey, string postData, string postDataContentType = "application/json")
        {
            ServicePointManager.ServerCertificateValidationCallback += new RemoteCertificateValidationCallback(ValidateServerCertificate);

            //
            //  MESSAGE CONTENT
            byte[] byteArray = Encoding.UTF8.GetBytes(postData);

            //
            //  CREATE REQUEST
            HttpWebRequest Request = (HttpWebRequest)WebRequest.Create(strUri);
            Request.Method = "POST";
            Request.KeepAlive = false;
            Request.ContentType = postDataContentType;
            Request.Headers.Add(string.Format("Authorization: key={0}", apiKey));
            Request.ContentLength = byteArray.Length;

            Stream dataStream = Request.GetRequestStream();
            dataStream.Write(byteArray, 0, byteArray.Length);
            dataStream.Close();

            //
            //  SEND MESSAGE
            try
            {
                WebResponse Response = Request.GetResponse();
                HttpStatusCode ResponseCode = ((HttpWebResponse)Response).StatusCode;
                if (ResponseCode.Equals(HttpStatusCode.Unauthorized) || ResponseCode.Equals(HttpStatusCode.Forbidden))
                {
                    Console.WriteLine("Unauthorized - need new token");                    
                }
                else if (!ResponseCode.Equals(HttpStatusCode.OK))
                {
                    Console.WriteLine("Response from web service isn't OK");                    
                }

                StreamReader Reader = new StreamReader(Response.GetResponseStream());
                string responseLine = Reader.ReadToEnd();
                Reader.Close();

                return responseLine;
            }
            catch (Exception ex)
            {
                Console.WriteLine("[ERROR] SendGCMNotification " + ex.Message);
            }
            return "error";
        }       
    }
}
