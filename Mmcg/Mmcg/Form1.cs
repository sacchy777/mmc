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
using System.Windows.Forms.Integration;

namespace Mmcg
{
    public partial class Form1 : Form
    {
        const string Version = "Music Memo Pad 0.1.0(alpha)";

        [DllImport("mmc.dll", CallingConvention = CallingConvention.Cdecl)]
        unsafe static extern int mmc_convert(string infilename, string outilename, [Out] StringBuilder msg);

        [DllImport("mmc.dll", CallingConvention = CallingConvention.Cdecl)]
        unsafe static extern int mmc_convert_string(string mml, string outilename, [Out] StringBuilder msg);

        [DllImport("mmc.dll", CallingConvention = CallingConvention.Cdecl)]
        unsafe static extern void mmc_version([Out] StringBuilder version);

        bool textChanged = false;
        bool onceOpenedOrSaved = false;
        //string currentFilename = "";
        //string currentDirectory = "";

        System.Diagnostics.Process player;
        bool playing = false;

        class Configuration
        {
            public string currentFilename = "";
            public string currentDirectory = "";
            public string [] players;
            public string [] args;
            public bool loaded = false;
            public Configuration()
            {
                players = new string[4];
                args = new string[4];
                for (int i = 0; i < 4; i++)
                {
                    players[i] = "";
                    args[i] = "";
                }
            }
            public void load()
            {
                if (File.Exists(System.IO.Path.GetDirectoryName(Application.ExecutablePath) + "\\test.ini"))
                {
                    //MessageBox.Show("found test.ini", "Debug");
                }
                else
                {
//                    MessageBox.Show(System.IO.Path.GetDirectoryName(
//                        Application.ExecutablePath), "\\\\\\\");
                }
                // de-serialize to be implemented
            }
            public void save()
            {
                // serialize to be implemented
            }
        }

        Configuration config;

        private void close_player()
        {
            if (!playing) return;
            try
            { 
                player.Kill();
                player.WaitForExit();
                playing = false;
            }
            catch (Exception)
            {
                //MessageBox.Show("Failed to kill player", "Debug");
            }

        }

        private Form2 form2;
        private Form3 form3 = null;
        private HelpForm helpForm;


        private void updateComboBox(){
            toolStripComboBox1.Items.Clear();
            toolStripComboBox1.Items.Add("[1]" + form2.getPlayer(1));
            toolStripComboBox1.Items.Add("[2]" + form2.getPlayer(2));
            toolStripComboBox1.Items.Add("[3]" + form2.getPlayer(3));
            toolStripComboBox1.Items.Add("[4]" + form2.getPlayer(4));
        }

        private void updateStatusBar()
        {            
            int index = textBox1.SelectionStart;
            int line = textBox1.GetLineFromCharIndex(index);
            Point point = textBox1.GetPositionFromCharIndex(index);
            point.X = 0;
            int column = index - textBox1.GetCharIndexFromPosition(point);
            toolStripStatusLabel1.Text = "("+(line+1)+","+(column+1)+")";
        }

        readonly string[] search_dirs = { System.IO.Path.GetDirectoryName(Application.ExecutablePath) + "\\", @"c:\Program Files (x86)\", @"c:\Program Files\" };
        readonly string [] filenames = {"ctplay.exe", @"Windows Media Player\wmplayer.exe" };
        readonly string[] args = { "--no-repeat", "/play /close" };
        private void setForm2DefaultPlayers(Form2 f)
        {
            int id = 1;
            foreach (string dir in search_dirs)
            {
                for (int i = 0; i < filenames.Length; i++)
                {
                    if (File.Exists(dir + filenames[i]) && id < 4)
                    {
                        f.setPlayer(id, dir + filenames[i], args[i]);
                        id++;
//                        MessageBox.Show("Found:" + dir + filenames[i], "Debug");
                    }
                }
            }
            
            //f.setPlayer(1, "ctplay.exe", "--no-repeat");
            //f.setPlayer(2, @"c:\Program Files (x86)\Windows Media Player\wmplayer.exe", "/play /close");
            //f.setPlayer(3, @"c:\Program Files (x86)\YAMAHA\MidRadio Player\MidRadio.exe", "");
            //f.setPlayer(4, "", "");
             
        }


        public Form1(string[] args)
        {
            InitializeComponent();
            /*
            ICSharpCode.AvalonEdit.TextEditor te = new ICSharpCode.AvalonEdit.TextEditor();
            ElementHost host = new ElementHost();
            host.Dock = DockStyle.Fill;
            host.Child = te;
            this.Controls.Add(host);
            */
            this.StartPosition = FormStartPosition.CenterScreen;
            config = new Configuration();
            form2 = new Form2();
            form2.Owner = this;

            setForm2DefaultPlayers(form2);

            updateComboBox();
            toolStripComboBox1.SelectedIndex = 0;
            updateStatusBar();


            if (args.Length == 1)
            {
                loadFile(args[0]);
            }
            else
            {
                config.load();
//                loadFile(config.currentFilename);
                config.currentFilename = "Untitled.mmp";
                config.currentDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Personal);
            }
        }

        private void play_music()
        {

            close_player();

            int result;
            StringBuilder buf = new StringBuilder(256);
            result = mmc_convert_string(textBox1.Text, "temp.mid", buf);
            textBox2.Text = buf.ToString();
            if (result != 0) return;

            int selected = toolStripComboBox1.SelectedIndex;
            string option = form2.getArguments(selected + 1);
            string program_name = form2.getPlayer(selected + 1);
            string filename = "temp.mid";
            string fullpath_filename = "\"" + Path.GetFullPath(filename) + "\"";

            player = new System.Diagnostics.Process();
            if (!form2.isShowPlayer())
            {
                player.StartInfo.CreateNoWindow = true;
                player.StartInfo.UseShellExecute = true;
                player.StartInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;
            }

            string[] execPaths = {"",Application.ExecutablePath};
            bool playresult = false;
            foreach(string path in execPaths){
                player.StartInfo.FileName = "\"" + path + program_name + "\"";
                player.StartInfo.Arguments = option + " " + fullpath_filename;
                try { 
                    playresult = player.Start();
                    if (playresult)
                    {
                        playing = true;
//                        MessageBox.Show(player.StartInfo.FileName,"player");
                        break;
                    }
                }
                catch (Exception)
                {
                    ; // do nothing
                }
            }
            if (!playresult)
            {
                textBox2.Text += "[ERROR]Failed to invoke player.\n";
            }

        }


        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            play_music();
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



        private void stopButton_Click(object sender, EventArgs e)
        {
            close_player();  
        }

        private void aboutToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            StringBuilder buf = new StringBuilder(256);
            mmc_version(buf);
            MessageBox.Show(Version + "\n" + buf.ToString() + "\nCopyright (c) 2013 sada.gussy (https://github.com/sadagussy)");
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            close_player();
            config.save();
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            play_music();

        }

 
        bool confirmDiscardOkay()
        {
            if (!textChanged) return true;
            DialogResult result = MessageBox.Show("Changes will be discarded.\nAre you sure?", "Confirmation", MessageBoxButtons.OKCancel, MessageBoxIcon.Question);
            if (result != DialogResult.OK) return false; else return true;
        }

        private void loadFile(string filename)
        {
            StreamReader sr = new StreamReader(filename, true);
            config.currentFilename = filename;
            config.currentDirectory = Path.GetDirectoryName(filename);
//            MessageBox.Show("dir="+currentDirectory,"dir");
            textBox1.Text = sr.ReadToEnd();
            onceOpenedOrSaved = true;
            textChanged = false;
            this.Text = Version + " " + config.currentFilename;
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (confirmDiscardOkay() == false) return;
            openFileDialog1.Title = "Open";            
            openFileDialog1.FileName = "Untitled.mmp";
            openFileDialog1.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Personal);
            openFileDialog1.Filter = "Music Memo Pad file|*.mmp|Text file|*.txt|All files(*.*)|*.*";
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                using (Stream fileStream = openFileDialog1.OpenFile())
                {
                    StreamReader sr = new StreamReader(fileStream, true);
                    config.currentFilename = openFileDialog1.FileName;
                    textBox1.Text = sr.ReadToEnd();
                    onceOpenedOrSaved = true;
                    textChanged = false;
                    this.Text = Version + " " + config.currentFilename;
                }
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (onceOpenedOrSaved)
            {
                using (Stream fileStream = new FileStream(config.currentFilename, FileMode.Create))
                using (StreamWriter sr = new StreamWriter(fileStream))
                {
                    sr.Write(textBox1.Text);
                }
                textChanged = false;
                onceOpenedOrSaved = true;
                config.currentFilename = saveFileDialog1.FileName;
                this.Text = Version + " " + config.currentFilename;
            }
            else
            {
                saveAsToolStripMenuItem_Click(sender, e);
            }
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveFileDialog1.Title = "Save As";
            saveFileDialog1.InitialDirectory = config.currentDirectory;
            saveFileDialog1.FileName = config.currentFilename;
            saveFileDialog1.Filter = "MML file(.mmp)|*.mmp|Text file|*.txt|All files(*.*)|*.*";
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                using (Stream fileStream = saveFileDialog1.OpenFile())
                using (StreamWriter sr = new StreamWriter(fileStream))
                {
                    sr.Write(textBox1.Text);
                }
                textChanged = false;
                onceOpenedOrSaved = true;
                config.currentFilename = saveFileDialog1.FileName;
                this.Text = Version + " " + config.currentFilename;
            }
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (confirmDiscardOkay() == false) return;
            textBox1.Text = "";
            config.currentFilename = "";
        }

        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!confirmDiscardOkay()) e.Cancel = true;
        }

        private void optionToolStripMenuItem_Click(object sender, EventArgs e)
        {
  
            form2.ShowDialog();
        }

        private void undoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (textBox1.CanUndo)
            {
                textBox1.Undo();
                textBox1.ClearUndo();
            }
        }
        // cut
        private void copyToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (textBox1.SelectedText != "") textBox1.Cut();
        }

        private void copyToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            if (textBox1.SelectedText != "") textBox1.Copy();
        }

        private void pasteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Clipboard.GetDataObject().GetDataPresent(DataFormats.Text))
            {
                textBox1.Paste();
            }
        }

        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            textBox1.Cut();
            Clipboard.Clear();
        }

        private void selectAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            textBox1.SelectAll();
        }

        private void stopToolStripMenuItem_Click(object sender, EventArgs e)
        {
            close_player();
        }

        private void openButton_Click(object sender, EventArgs e)
        {
            openToolStripMenuItem_Click(sender, e);
        }

        private void saveButton_Click(object sender, EventArgs e)
        {
            saveToolStripMenuItem_Click(sender, e);
        }

        private void textBox1_CursorChanged(object sender, EventArgs e)
        {

        }

        private void findToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (form3 == null || form3.IsDisposed)
            {
                form3 = new Form3(this.textBox1);
                form3.Owner = this;
                form3.Show();
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            this.Text = Version + " " + config.currentFilename + "*";
            textChanged = true;
        }

        private void helpToolStripMenuItem3_Click(object sender, EventArgs e)
        {
            helpForm = new HelpForm();
            helpForm.Owner = this;
            helpForm.ShowDialog();
        }

        private void textBox1_MouseClick(object sender, MouseEventArgs e)
        {
            updateStatusBar();
        }

        private void textBox1_KeyDown(object sender, KeyEventArgs e)
        {
            
        }

        private void textBox1_KeyUp(object sender, KeyEventArgs e)
        {
            updateStatusBar();
        }

        private void Form1_DragDrop(object sender, DragEventArgs e)
        {
            string filename = ((string[])e.Data.GetData(DataFormats.FileDrop))[0];
            loadFile(filename);
        }

        private void Form1_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                e.Effect = DragDropEffects.Copy;
            }
        }

    }
}
