using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO;
using System.Reflection;
using System.Xml;

namespace Mmcg
{
    public partial class Form1 : Form
    {
        [DllImport("mmc.dll", CallingConvention = CallingConvention.Cdecl)]
        unsafe static extern int mmc_convert(string infilename, string outilename, [Out] StringBuilder msg);

        [DllImport("mmc.dll", CallingConvention = CallingConvention.Cdecl)]
        unsafe static extern int mmc_convert_string(string mml, string outilename, [Out] StringBuilder msg);

        [DllImport("mmc.dll", CallingConvention = CallingConvention.Cdecl)]
        unsafe static extern void mmc_version([Out] StringBuilder version);

        const string Version = "mmcg 0.0(alpha)";


        System.Diagnostics.Process player;
        bool playing = false;

        private void close_player()
        {
            try
            {
                if (playing) player.Kill();
            }
            catch (Exception)
            {
                ;
            }

        }

        public Form1()
        {
            InitializeComponent();
        }

        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            int result;
            StringBuilder buf = new StringBuilder(256);
            result = mmc_convert_string(richTextBox1.Text, "temp.mid", buf);
            //            int result = mmc_convert("a.mml", "temp.mid", buf);

            /*
  string program_name = "\"c:\\Program Files (x86)\\Windows Media Player\\wmplayer.exe\"";
  string option = "/play";
   */
            string program_name = "\"c:\\Program Files (x86)\\YAMAHA\\MidRadio Player\\MidRadio.exe\"";
             string option = "";
            /*
            string option = "--no-repeat";
            string program_name = "ctplay.exe";
            */
             string filename = "temp.mid";
            try
            {
                if (playing) player.Kill();
            }
            catch (Exception)
            {
                ;
            }
            player = System.Diagnostics.Process.Start(program_name, option + " " + filename);
            playing = true;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
        }

        private void toolStripComboBox1_Click(object sender, EventArgs e)
        {

        }

        private void helpToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void stopButton_Click(object sender, EventArgs e)
        {
            close_player();  
        }

        private void aboutToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            StringBuilder buf = new StringBuilder(256);
            mmc_version(buf);
            MessageBox.Show(Version + "\n" + buf.ToString() + "\nCopyright (c) 2013 sada.gussy (sada dot gussy at gmail dot com)");
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            close_player();
        }

    }
}
