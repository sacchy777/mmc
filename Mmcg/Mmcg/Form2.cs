using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Mmcg
{
    public partial class Form2 : Form
    {
        public Form2()
        {
            InitializeComponent();
        }

        public bool isShowPlayer()
        {
            return checkBox2.Checked;
        }

        public bool isShowWarning()
        {
            return checkBox1.Checked;
        }
        public string getPlayer(int id)
        {
            switch(id){
                case 1: return textBox1.Text;
                case 2: return textBox3.Text;
                case 3: return textBox5.Text;
                case 4: return textBox7.Text;
            }
            return "";
        }

        public void setPlayer(int id, string text, string arg)
        {
            switch (id)
            {
                case 1: textBox1.Text = text; textBox2.Text = arg;  break;
                case 2: textBox3.Text = text; textBox4.Text = arg; break;
                case 3: textBox5.Text = text; textBox6.Text = arg; break;
                case 4: textBox7.Text = text; textBox8.Text = arg; break;
            }
        }


        public string getArguments(int id)
        {
            switch (id)
            {
                case 1: return textBox2.Text;
                case 2: return textBox4.Text;
                case 3: return textBox6.Text;
                case 4: return textBox8.Text;
                default: return "";
            }
        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void textBox7_TextChanged(object sender, EventArgs e)
        {

        }

        private void folderBrowserDialog1_HelpRequest(object sender, EventArgs e)
        {

        }

        private void openfiledialog_setting()
        {
            openFileDialog1.Title = "Open";
            openFileDialog1.FileName = "*.exe";
            openFileDialog1.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFilesX86);
            openFileDialog1.Filter = "EXE file|*.exe|All files(*.*)|*.*";
        }

        private void button2_Click(object sender, EventArgs e)
        {
            openfiledialog_setting();
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox3.Text = openFileDialog1.FileName;
            }
        }

        private void button3_Click_1(object sender, EventArgs e)
        {
            openfiledialog_setting();
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox5.Text = openFileDialog1.FileName;
            }
        }

        private void button4_Click_1(object sender, EventArgs e)
        {
            openfiledialog_setting();
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox7.Text = openFileDialog1.FileName;
            }

        }
    }
}
