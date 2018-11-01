using System;
using System.IO;

namespace DeltaruneAud
{
    class Program
    {
        static void Main(string[] args)
        {
            const string BadData = "Error: Invalid input!";

            using (var In = File.OpenRead(args[0]))
            using (var Rd = new BinaryReader(In))
            {
                if (Rd.ReadInt32() != 0x4d524f46) throw new InvalidDataException(BadData); 
                if (Rd.ReadInt32() != In.Length - In.Position) throw new InvalidDataException(BadData);
                if (Rd.ReadInt32() != 0x4f445541) throw new InvalidDataException(BadData);
                if (Rd.ReadInt32() != In.Length - In.Position) throw new InvalidDataException(BadData);

                var NumOfFiles = Rd.ReadInt32();

                var Ofs = new int[NumOfFiles];

                for (int i = 0; i < NumOfFiles; i++)
                    Ofs[i] = Rd.ReadInt32();

                var OutDir = args[0].Substring(0, args[0].LastIndexOf('.'));

                Directory.CreateDirectory(OutDir);

                for (int i = 0; i < NumOfFiles; i++)
                {
                    var Pos = Ofs[i];
                    In.Position = Pos;
                    var Size = Rd.ReadInt32();
                    var Name = $"0x{Pos:x}-0x{Pos+Size:x}";
                    Console.WriteLine($"Ripping WAV at {Name}...");
                    File.WriteAllBytes($"{OutDir}/{Name}.wav", Rd.ReadBytes(Size));
                }

                Console.WriteLine("\nDone!");
            }
        }
    }
}