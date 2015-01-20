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
        public string getArguments(int id)
        {
            switch (id)
            {
                case 1: return textBox2.Text;
                case 2: return textBox4.Text;
                case 3: return textBox6.Text;
                case 4: return textBox8.Text;
            }
            return "";
        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void textBox7_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
