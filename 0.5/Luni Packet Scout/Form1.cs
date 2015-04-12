using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net.Sockets;
using System.Net;

namespace Luni_Packet_Scout_Cs 
{
    public partial class Form1 : Form {
        private const string endl = "\n";
        private UdpClient socket;
        private bool listening = false;
        private static string buff = "";
        
        public Form1() {
            InitializeComponent();
        }

        private void downloadbuffer() {
            rblog.Text += buff;
            buff = "";
        }

        private void btnstart_Click(object sender, EventArgs e) {
            if (!listening) {
                rblog.Text = "";
                socket = new UdpClient((int)ndoor.Value);
                tmbuf.Start();
                bgw.RunWorkerAsync();
                btnstart.Text = "Stop";
                listening = true;
            } else {
                tmbuf.Stop();
                downloadbuffer();
                bgw.CancelAsync();
                socket.Close();
                rblog.Text += "Stopped!";
                listening = false;
                btnstart.Text = "Start";
            }
        }

        private void bgw_DoWork(object sender, DoWorkEventArgs e) {
            buff = "Listening on door " + ndoor.Value + endl;

            while (socket != null) {
                if (socket.Available > 0) {
                    IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 0);
                    Byte[] recvbytes= socket.Receive(ref ipep);

                    buff += "Client (" + ipep.Address + ":" + ipep.Port + "): ";
                    foreach (Byte b in recvbytes) buff += b.ToString() + " ";
                    buff += endl;
                }
            }
        }

        private void tmbuf_Tick(object sender, EventArgs e) {
            downloadbuffer();
        }
    }
}
