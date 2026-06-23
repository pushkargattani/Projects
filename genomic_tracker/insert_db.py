import mysql.connector
import csv

# connect to MySQL
conn = mysql.connector.connect(
    host="localhost",
    user="root",
    password="Kalyani@1508",
    database="genomic_tracker"
)
cursor = conn.cursor()

# clear old data
cursor.execute("DELETE FROM mutation_results")
cursor.execute("DELETE FROM patients")
cursor.execute("DELETE FROM mutation_frequencies")
conn.commit()
print("Old data cleared")

# insert patients
with open("results.csv", "r") as f:
    reader = csv.DictReader(f)
    for row in reader:
        cursor.execute("""
            INSERT INTO patients VALUES (%s,%s,%s,%s,%s,%s,%s)
        """, (
            row["patient_id"],
            int(row["age"]),
            int(row["dna_length"]),
            int(row["total_mutations"]),
            int(row["unique_mutations"]),
            float(row["mutation_density"]),
            float(row["risk_score"])
        ))
conn.commit()
print("Patients inserted!")

# insert mutation results
with open("mutation_results.csv", "r") as f:
    reader = csv.DictReader(f)
    for row in reader:
        cursor.execute("""
            INSERT INTO mutation_results
            (patient_id, mutation_name, position, algorithm)
            VALUES (%s,%s,%s,%s)
        """, (
            row["patient_id"],
            row["mutation_name"],
            int(row["position"]),
            row["algorithm"]
        ))
conn.commit()
print("Mutation results inserted!")

# insert frequencies
with open("frequencies.csv", "r") as f:
    reader = csv.DictReader(f)
    for row in reader:
        cursor.execute("""
            INSERT INTO mutation_frequencies VALUES (%s,%s)
        """, (
            row["mutation_name"],
            int(row["total_count"])
        ))
conn.commit()
print("Frequencies inserted!")

cursor.close()
conn.close()
print("All done! Check MySQL Workbench.")