using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Data.SqlClient;
using System.Net;
using System.IO;

namespace LogView
{
    public partial class Form1 : Form
    {
        string SearchCondition;
        string SearchInfo = string.Empty;

        private string DBIp = "192.168.0.192";
        private string DBName = "Caribe";
        private string DBId = "Caribe";
        private string DBPass = "Mobile099!";

        public Form1()
        {
            InitializeComponent();
            InitMain();
        }
        private void InitMain()
        {
            SearchCondition = this.searchtype.Text;
        }
        private void tbxName_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter) 
                button_search_Click(sender, e);
        }
        private void searchtype_SelectedIndexChanged(object sender, EventArgs e)
        {
            SearchCondition = this.searchtype.Text;   
        }

        private void button_search_Click(object sender, EventArgs e)
        {
            try            
            {
                if (SearchCondition.Equals("All"))
                {
                    LoadUserLogAll();
                }
                else
                {
                    SearchInfo = this.targetinfo.Text;
                    if (SearchInfo.Length == 0)
                    {
                        MessageBox.Show("Error:찾을대상을입력하세요.");
                        return;
                    }

                    if (SearchCondition.Equals("NickName"))
                    {
                        LoadUserLogNickName(SearchInfo);
                    }
                    else if (SearchCondition.Equals("UUID"))
                    {
                        LoadUserLogUUID(SearchInfo);
                    }
                    else if (SearchCondition.Equals("IDAccount"))
                    {
                        LoadUserLogIDAccount(SearchInfo);
                    }
                }                
            }
            catch(Exception ex )
            {
                MessageBox.Show(ex.Message, "[ERROR]", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        private void LoadUserLogNickName(string SearchInfo)
        {
            try
            {
                string idaccount = FindUseridaccountByNickName(SearchInfo);

                if (idaccount.Length == 0 || idaccount == string.Empty)
                {
                    MessageBox.Show("Error:대상을 찾을 수 없습니다..");
                    return;
                }
                LoadUserLogIDAccount(idaccount);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "[ERROR]", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        private void LoadUserLogUUID(string SearchInfo)
        {
            try
            {
                string idaccount = FindUseridaccountByUUID(SearchInfo);

                if (idaccount.Length == 0 || idaccount == string.Empty)
                {
                    MessageBox.Show("Error:대상을 찾을 수 없습니다..");
                    return;
                }

                LoadUserLogIDAccount(idaccount);
            }            
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "[ERROR]", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }            
        }
        private void LoadUserLogAll()
        {
            try
            {
                this.listViewLog.Items.Clear();
                this.listViewLog.BeginUpdate();

                using (SqlConnection con = new SqlConnection())
                {
                    con.ConnectionString = string.Format("Server={0};DataBase={1};Uid={2};Pwd={3}", this.DBIp, this.DBName, this.DBId, this.DBPass);

                    string sqlquery = "SELECT idaccount, log_index,logtype,Nickname,LogContext,logtime FROM [Caribe].[dbo].[tbl_userLog] ORDER BY log_index";                    
                    
                    SqlCommand cmd = new SqlCommand(sqlquery, con);
                    con.Open();
                    SqlDataReader rd = cmd.ExecuteReader();

                    while (rd.Read())
                    {
                        ListViewItem viewitem = new ListViewItem();

                        viewitem.Text = rd["log_index"].ToString();
                        viewitem.SubItems.Add(rd["idaccount"].ToString());
                        viewitem.SubItems.Add(rd["Nickname"].ToString());

                        string logtype = rd["logtype"].ToString();
                        string LogType = GetLogType(logtype);
                        viewitem.SubItems.Add(LogType);

                        viewitem.SubItems.Add(rd["LogContext"].ToString());
                        viewitem.SubItems.Add(rd["logtime"].ToString());

                        this.listViewLog.Items.Add(viewitem);                        
                    }
                    rd.Close();
                    con.Close();
                    this.listViewLog.AutoResizeColumns(ColumnHeaderAutoResizeStyle.HeaderSize);
                    this.listViewLog.EndUpdate();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "[ERROR]", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        private void LoadUserLogIDAccount(string idaccount)
        {            
            try
            {                
                this.listViewLog.Items.Clear();
                this.listViewLog.BeginUpdate();
                using (SqlConnection con = new SqlConnection())
                {
                    con.ConnectionString = string.Format("Server={0};DataBase={1};Uid={2};Pwd={3}", this.DBIp, this.DBName, this.DBId, this.DBPass);                    

                    //string sqlquery = "SELECT log_index,logtype,Nickname,LogContext,logtime FROM [Caribe].[dbo].[tbl_userLog] ORDER BY log_index";
                    string sqlquery = "SELECT idaccount, log_index,logtype,Nickname,LogContext,logtime FROM [Caribe].[dbo].[tbl_userLog] WHERE idaccount = @IDName ORDER BY log_index";
                    SqlCommand cmd = new SqlCommand(sqlquery, con);
                    cmd.Parameters.AddWithValue("@IDName", idaccount);

                    con.Open();
                    SqlDataReader rd = cmd.ExecuteReader();

                    while (rd.Read())
                    {
                        ListViewItem viewitem = new ListViewItem();

                        viewitem.SubItems.Add(rd["log_index"].ToString());
                        viewitem.SubItems.Add(rd["idaccount"].ToString());
                        viewitem.SubItems.Add(rd["Nickname"].ToString());
                        
                        string logtype = rd["logtype"].ToString();
                        string LogType = GetLogType(logtype);
                        viewitem.SubItems.Add(LogType);

                        viewitem.SubItems.Add(rd["LogContext"].ToString());
                        viewitem.SubItems.Add(rd["logtime"].ToString());

                        this.listViewLog.Items.Add(viewitem);                        
                    }
                    rd.Close();
                    con.Close();
                    this.listViewLog.AutoResizeColumns(ColumnHeaderAutoResizeStyle.HeaderSize);
                    this.listViewLog.EndUpdate();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "[ERROR]", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        private string GetLogType(string logtype)
        {
            try
            {
                int type = Convert.ToInt32(logtype);
                switch (type)
                {
                    case 1: return "xULog_Account_Create";
                    case 2: return "xULog_Account_Login";
                    case 3: return "xULog_Account_Save";
                    case 4: return "xULog_Account_Logout";
                    case 5: return "xULog_Account_Duplicate_connect";
                    case 6: return "xULog_Account_Level_Up";
                    case 7: return "xULog_Quest_Reward";
                    case 8: return "xULog_Cloud_Open";
                    case 9: return "xULog_User_Spot_Recon";
                    case 10: return "xULog_User_Spot_Combat";
                    case 11: return "xULog_Use_Trade";
                    case 12: return "xULog_Trade_Call_Gem";
                    case 13: return "xULog_ShopList_Call_Gem";
                    case 14: return "xULog_Trade_Call_Time";
                    case 15: return "xULog_ShopList_Call_Time";
                    case 16: return "xULog_Skill_Tree_Research";
                    case 17: return "xULog_Hero_Level_Up";
                    case 18: return "xULog_Hero_Skill_Level_Up";
                    case 19: return "xULog_Hero_Legion_Level_Up";
                    case 20: return "xULog_Hero_Gatcha_Normal";
                    case 21: return "xULog_Hero_Gatcha_Royal";
                    case 22: return "xULog_Buy_Items";
                    case 23: return "xULog_Sell_Items";
                    case 24: return "xULog_Buy_Gem";
                    case 25: return "xULog_Buy_Gem_Subscribe";
                    case 26: return "xULog_Use_Gem";
                    case 100: return "xULog_System = 100";
                    case 101: return "xULog_System_Unkown_idpacket";
                    case 102: return "xULog_System_Use_Cheat";
                    case 103: return "xULog_Unkown_idpacket";
                    case 104: return "xULog_MAX = 200";
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "[ERROR]", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            return string.Empty;
        }
        private string FindUseridaccountByNickName(string NickName)
        {
            string IdAccount = string.Empty;
            try
            {                
                using (SqlConnection con = new SqlConnection())
                {
                    con.ConnectionString = string.Format("Server={0};DataBase={1};Uid={2};Pwd={3}", this.DBIp, this.DBName, this.DBId, this.DBPass);
                    //MessageBox.Show(con.State.ToString());

                    string sqlquery = "SELECT idaccount  FROM [Caribe].[dbo].[tbl_usertable ] WHERE szNickName = @IDName";
                    SqlCommand cmd = new SqlCommand(sqlquery, con);
                    cmd.Parameters.AddWithValue("@IDName", NickName);

                    con.Open();
                    SqlDataReader rd = cmd.ExecuteReader();

                    if (rd.Read())
                    {
                        IdAccount  = rd["idaccount"].ToString();                        
                    }
                    rd.Close();
                    con.Close();

                    return IdAccount;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "[ERROR]", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            return string.Empty;
        }
        private string FindUseridaccountByUUID(string UUID)
        {
            string IdAccount = string.Empty;
            try
            {
                using (SqlConnection con = new SqlConnection())
                {
                    con.ConnectionString = string.Format("Server={0};DataBase={1};Uid={2};Pwd={3}", this.DBIp, this.DBName, this.DBId, this.DBPass);
                    //MessageBox.Show(con.State.ToString());

                    string sqlquery = "SELECT idaccount  FROM [Caribe].[dbo].[tbl_usertable ] WHERE szUUID = @IDName";
                    SqlCommand cmd = new SqlCommand(sqlquery, con);
                    cmd.Parameters.AddWithValue("@IDName", UUID);

                    con.Open();
                    SqlDataReader rd = cmd.ExecuteReader();

                    if (rd.Read())
                    {
                        IdAccount = rd["idaccount"].ToString();
                    }
                    rd.Close();
                    con.Close();

                    return IdAccount;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "[ERROR]", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            return string.Empty;;
        }

        private void listViewLog_ColumnClick(object sender, ColumnClickEventArgs e)
        {
            //정렬을 위하여 사용 됨.
            if (this.listViewLog.Sorting == System.Windows.Forms.SortOrder.Ascending || listViewLog.Sorting == System.Windows.Forms.SortOrder.None)
            {
                this.listViewLog.ListViewItemSorter = new ListViewItemComparer(e.Column, "desc");
                listViewLog.Sorting = System.Windows.Forms.SortOrder.Descending;
            }
            else
            {
                this.listViewLog.ListViewItemSorter = new ListViewItemComparer(e.Column, "asc");
                listViewLog.Sorting = System.Windows.Forms.SortOrder.Ascending;
            }

            listViewLog.Sort();
        }
    }
    class ListViewItemComparer : IComparer
    {
        private int col;
        public string sort = "asc";
        public ListViewItemComparer()
        {
            col = 0;
        }
        /// <summary>
        /// 컬럼과 정렬 기준(asc, desc)을 사용하여 정렬 함.
        /// </summary>
        /// <param name="column">몇 번째 컬럼인지를 나타냄.</param>
        /// <param name="sort">정렬 방법을 나타냄. Ex) asc, desc</param>
        public ListViewItemComparer(int column, string sort)
        {
            col = column;
            this.sort = sort;
        }
        public int Compare(object x, object y)
        {
            if (sort.Equals( "asc"))
                return String.Compare(((ListViewItem)x).SubItems[col].Text, ((ListViewItem)y).SubItems[col].Text);
            else
                return String.Compare(((ListViewItem)y).SubItems[col].Text, ((ListViewItem)x).SubItems[col].Text);
        }
    }
 
//사용할 이벤트가 있는 곳에 다음과 같이 처리한다.
}
