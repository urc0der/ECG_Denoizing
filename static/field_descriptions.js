const fieldDescriptions = {
    age: "Age in years. Typical range: 29-77.",
    sex: "Sex: 1 = Male, 0 = Female.",
    cp: "Chest pain type (0-3): 0 = typical angina, 1 = atypical angina, 2 = non-anginal pain, 3 = asymptomatic.",
    trestbps: "Resting blood pressure (mm Hg). Typical range: 94-200.",
    chol: "Serum cholesterol (mg/dl). Typical range: 126-564.",
    fbs: "Fasting blood sugar > 120 mg/dl: 1 = True, 0 = False.",
    restecg: "Resting electrocardiographic results (0-2): 0 = normal, 1 = ST-T wave abnormality, 2 = left ventricular hypertrophy.",
    thalach: "Maximum heart rate achieved. Typical range: 71-202.",
    exang: "Exercise induced angina: 1 = Yes, 0 = No.",
    oldpeak: "ST depression induced by exercise relative to rest. Typical range: 0.0-6.2.",
    slope: "Slope of the peak exercise ST segment (0-2): 0 = upsloping, 1 = flat, 2 = downsloping.",
    ca: "Number of major vessels (0-3) colored by fluoroscopy.",
    thal: "Thalassemia (0-3): 0 = unknown, 1 = normal, 2 = fixed defect, 3 = reversible defect."
};

function showDescription(event, field) {
    var desc = fieldDescriptions[field] || "No description available.";
    var tooltip = document.getElementById('descTooltip');
    tooltip.innerHTML = desc;
    tooltip.style.display = 'block';
    var rect = event.target.getBoundingClientRect();
    var scrollY = window.scrollY || window.pageYOffset;
    // Position to the immediate right of the button
    tooltip.style.left = (rect.right + 8) + 'px';
    tooltip.style.top = (rect.top + scrollY) + 'px';
}

function hideDescription() {
    var tooltip = document.getElementById('descTooltip');
    tooltip.style.display = 'none';
}

document.getElementById('predictForm').addEventListener('reset', function(event) {
    event.preventDefault(); 
    const form = event.target;
    Array.from(form.elements).forEach(element => {
        if (element.type === 'number' || element.type === 'text') {
            element.value = '';
        }
    });
});
