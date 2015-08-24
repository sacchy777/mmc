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
    public partial class Form3 : Form
    {
        private TextBox _target;
        private int lastPos;
        public Form3(TextBox target)
        {
            InitializeComponent();
            _target = target;
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            int len = textBox1.Text.Length;
            if (len == 0) return;
            int currentPos = _target.SelectionStart;
            if (currentPos == lastPos) currentPos++;
            int found = _target.Text.IndexOf(textBox1.Text, currentPos);
            if (found == -1) return;
            lastPos = found;
            _target.Select(found, len);
            _target.ScrollToCaret();
            _target.Focus();
        }
    }
}
