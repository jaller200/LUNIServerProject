using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace Luni_Packet_Scout_Cs
{
    static class Program
    {
        /// <summary>
        /// Main entry point of the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }
    }
}
