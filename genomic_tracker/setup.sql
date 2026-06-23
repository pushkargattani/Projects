CREATE DATABASE IF NOT EXISTS genomic_tracker;
USE genomic_tracker;

CREATE TABLE IF NOT EXISTS patients (
    patient_id       VARCHAR(10) PRIMARY KEY,
    age              INT,
    dna_length       INT,
    total_mutations  INT,
    unique_mutations INT,
    mutation_density DOUBLE,
    risk_score       DOUBLE
);

CREATE TABLE IF NOT EXISTS mutation_results (
    id            INT AUTO_INCREMENT PRIMARY KEY,
    patient_id    VARCHAR(10),
    mutation_name VARCHAR(20),
    position      INT,
    algorithm     VARCHAR(20),
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id)
);

CREATE TABLE IF NOT EXISTS mutation_frequencies (
    mutation_name VARCHAR(20) PRIMARY KEY,
    total_count   INT
);

-- Verify tables created
SHOW TABLES;

-- Sample analytics queries

-- Top 10 highest risk patients
SELECT patient_id, age, risk_score
FROM patients
ORDER BY risk_score DESC
LIMIT 10;

-- Average mutations by age group
SELECT
    CASE
        WHEN age < 30 THEN '20-29'
        WHEN age < 50 THEN '30-49'
        WHEN age < 70 THEN '50-69'
        ELSE '70-80'
    END AS age_group,
    COUNT(*) as total_patients,
    AVG(total_mutations) as avg_mutations,
    AVG(risk_score) as avg_risk
FROM patients
GROUP BY age_group
ORDER BY age_group;

-- Most common mutations
SELECT mutation_name, total_count
FROM mutation_frequencies
ORDER BY total_count DESC;

-- Window function: rank patients by risk
SELECT
    patient_id,
    age,
    risk_score,
    RANK() OVER (ORDER BY risk_score DESC) as risk_rank,
    AVG(risk_score) OVER () as overall_avg_risk
FROM patients
LIMIT 10;

-- Age group mutation density comparison
SELECT
    CASE
        WHEN age < 30 THEN '20-29'
        WHEN age < 50 THEN '30-49'
        WHEN age < 70 THEN '50-69'
        ELSE '70-80'
    END AS age_group,
    AVG(mutation_density) as avg_density,
    MAX(risk_score) as max_risk,
    MIN(risk_score) as min_risk
FROM patients
GROUP BY age_group
ORDER BY avg_density DESC;