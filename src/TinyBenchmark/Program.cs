using CommandLine;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace TinyBenchmark
{
    class Options
    {
        [Option('g', "group", Required = false, HelpText = "set the group number.")]
        public int Group { get; set; } = 5;

        [Option('r', "random", Required = false, HelpText = "Randomize Counter.")]
        public bool IsRandomCounter { get; set; } = false;
    }

    internal class Program
    {
        [DllImport("kernel32.dll")]
        public static extern uint GetCurrentThreadId();

        private static readonly object fileLock = new object();

        // cpu counter
        static void ThreadFunction1(object obj)
        {
            var (fileName, grp, id, randomCounter) = ((string, int, int, bool))obj;
            uint nativeThreadId = GetCurrentThreadId();

            lock (fileLock)
            {
                File.AppendAllText(fileName, $"{Process.GetCurrentProcess().Id}, {GetCurrentThreadId()}, {Thread.CurrentThread.ManagedThreadId}. {grp}, {id}\n");
            }

            var denominator = 1;
            if (randomCounter)
            {
                denominator = (new Random()).Next(2, 6);
            }
            var counter = int.MaxValue / denominator;
            Console.WriteLine($"id={id}, denominator={denominator} ");
            while (counter-- > 0)
                ;
        }

        static void RunWithOptions(Options opts)
        {
            Console.WriteLine($"Group number set to: {opts.Group}");

            string fileName = $"threads_{Process.GetCurrentProcess().Id}.csv";
            lock (fileLock)
            {
                File.WriteAllText(fileName, "Process ID, Native Thread ID, Managed Thread ID, Group, Serial Number\n");
            }

            // cpu counter
            for (int grp = 0; grp < opts.Group; grp++)
            {
                var thdCount = (int)Math.Pow(2, grp);
                var threads = new List<Thread>();
                for (int j = 0; j < thdCount; j++)
                {
                    var thread = new Thread(new ParameterizedThreadStart(ThreadFunction1));
                    thread.Start((fileName, grp, j, opts.IsRandomCounter));
                    threads.Add(thread);
                }
                foreach (var thread in threads)
                {
                    thread.Join();
                }
                Console.WriteLine();
            }
        }

        static void HandleParseError(IEnumerable<Error> errs)
        {
        }

        static void Main(string[] args)
        {
            Process.GetCurrentProcess().ProcessorAffinity = (nint)0x2;

            Parser.Default.ParseArguments<Options>(args)
                .WithParsed<Options>(opts => RunWithOptions(opts))
                .WithNotParsed<Options>(errs => HandleParseError(errs));
        }
    }
}